/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2022 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License              */
/*  along with SCIP; see the file COPYING. If not visit scipopt.org.         */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file   pricer_srr.c
 * @brief  SRR variable pricer
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "scip/scipdefplugins.h"
#include "scip/scip.h"
#include "scip/struct_tree.h"
#include "scip/tree.h"
#include "pricer_srr.h"
#include "probdata_srr.h"
#include "lemon_wrapper.h"
#include "cons_matchonround.h"

/**@name Pricer properties
 *
 * @{
 */

#define PRICER_NAME            "matchingpricer"
#define PRICER_DESC            "pricer for srr"
#define PRICER_PRIORITY        0
#define PRICER_DELAY           TRUE          /* only call pricer if all problem variables have non-negative reduced costs */

#define EVENTHDLR_NAME         "srrpricereventvardeleted"
#define EVENTHDLR_DESC         "event handler for variable deleted event"


/**@} */


/*
 * Data structures
 */


/*
 * Event handler
 */

/** execution method of event handler */
static
SCIP_DECL_EVENTEXEC(eventExecVardeleted)
{
   SCIP_VAR* var;
   int idx;

   assert(SCIPeventGetType(event) == SCIP_EVENTTYPE_VARDELETED);
   var = SCIPeventGetVar(event);

   assert( var != NULL );
   assert( SCIPvarIsDeletable(var) );

   /* get index of variable in stablesets array */
   idx = (int)(size_t) SCIPvarGetData(var);

   SCIPdebugMsg(scip, "remove variable %s [%d]\n", SCIPvarGetName(var), idx);

   /* remove variable from stablesets array and release it */
   SCIP_CALL( SCIPreleaseVar(scip, &var) );

   return SCIP_OKAY;
}/*lint !e715*/


/**@name Local methods
 *
 * @{
 */

static
SCIP_RETCODE solvePricingSRR(
   SCIP* scip,
   SCIP_PRICER* pricer,
   SCIP_Bool farkas,
   SCIP_Bool* result
)
{
   int i;
   int j;
   int r;
   int k;
   SCIP_Real roundpi;
   int nteams;
   int nrounds;
   int nedges;
   SCIP_Real* coefs;
   SCIP_CONS** conssmatchingonround;
   SCIP_CONS** conssmatchisplayed;
   SCIP_CONS* cons;
   SCIP_CONS** conss;
   int nconss;
   SCIP_Real* weights;
   SCIP_Bool* edgeexists;
   SCIP_Real solutionvalue;
   SCIP_Real matchingvalue;
   int* solution;
   SCIP_VAR* var;
   SCIP_NODE* node;
   SCIP_Bool exists;
   int ii;
   int jj;
   int rr;
   SCIP_Bool permitted;
   int ncolsadded;

   assert( scip != NULL );
   assert( pricer != NULL );

   SCIP_CALL( SCIPgetProblemDataSRR(scip, &nteams, &nrounds, &coefs, &conssmatchingonround,
      &conssmatchisplayed) );

   assert( nteams > 0 );
   assert( nteams % 2 == 0 );
   assert( nrounds == nteams - 1 );
   assert( coefs != NULL );
   assert( conssmatchingonround != NULL );
   assert( conssmatchisplayed != NULL );

   nedges = (nteams / 2) * (nteams - 1);

   SCIP_CALL( SCIPallocBufferArray(scip, &conss, 1) );  /* Only one constraint is added per node */
   SCIP_CALL( SCIPallocBufferArray(scip, &weights, nedges) );
   SCIP_CALL( SCIPallocBufferArray(scip, &edgeexists, nedges) );
   SCIP_CALL( SCIPallocBufferArray(scip, &solution, nteams) );

   *result = SCIP_DIDNOTRUN;
   ncolsadded = 0;

   for (r = 0; r < nrounds; ++r)
   {

      SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchingonround[r], &cons) );
      roundpi = farkas?
            SCIPgetDualfarkasLinear(scip, cons):
            SCIPgetDualsolLinear(scip, cons);

      k = -1;
      for (i = 0; i < nteams; ++i)
      {
         for (j = i + 1; j < nteams; ++j)
         {
            ++k;
            /* edgeexists and weights arrays follow the natural unordered edge sorting,
            * that (i,j) < (i',j') if i < i', or i = i' and j < j'.
            */

            /* is this a legal edge? */
            node = SCIPgetCurrentNode(scip);
            exists = TRUE;
            while (node != NULL)
            {
               if ( SCIPnodeGetNAddedConss(node) > 0 )
               {
                  assert( SCIPnodeGetNAddedConss(node) == 1 );
                  SCIPnodeGetAddedConss(node, conss, &nconss, 1);

                  SCIP_CALL( SCIPgetConstraintDataSRRmor(conss[0], &ii, &jj, &rr, &permitted) );

                  /* is edge {i, j} on round `r` permitted? */
                  if ( permitted )
                  {
                     if ( rr == r )
                     {
                        if ( ii == i && jj == j )
                        {
                           /* Must be the case */
                        }
                        else if ( ii == i || jj == j )
                        {
                           /* i and j must play against one other. */
                           exists = FALSE;
                           break;
                        }
                     }
                     else /* rr != r */
                     {
                        if ( ii == i && jj == j )
                        {
                           /* i and j must play on r, not on rr. */
                           exists = FALSE;
                           break;
                        }
                     }
                  }
                  else
                  {
                     if ( rr == r )
                     {
                        if ( ii == i && jj == j )
                        {
                           /* i and j must not play against one other. */
                           exists = FALSE;
                           break;
                        }
                     }
                  }
               }

               node = SCIPnodeGetParent(node);
            }
            edgeexists[k] = exists;  /* No branching decisions, yet ... */

            SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchisplayed[k], &cons) );
            weights[k] = farkas?
               SCIPgetDualfarkasLinear(scip, cons):
               SCIPgetDualsolLinear(scip, cons) - coefs[j + i * nteams + r * nteams * nteams];
            assert( coefs[i + j * nteams + r * nteams * nteams] == coefs[j + i * nteams + r * nteams * nteams] );
         }
      }

      SCIPmatchingSolveSRR(nteams, weights, edgeexists, &solutionvalue, solution);

      /* If this matching is improving, add it! */
      if ( SCIPisGT(scip, roundpi + solutionvalue, 0.0) )
      {
         /*
         SCIPdebugMessage("Adding ");
         for (i = 0; i < nteams; ++i)
            if ( i < solution[i] ) SCIPdebugMessage("%d-%d, ", i, solution[i]);
         SCIPdebugMessage("on round %d (Farkas %d, Roundpi %lf, Solution %lf, Total %lf)\n", r, farkas, roundpi,
            solutionvalue, roundpi + solutionvalue);
         */

         matchingvalue = 0.0;
         for (i = 0; i < nteams; ++i)
         {
            j = solution[i];
            assert( i == solution[j] );
            if ( i < j )
               matchingvalue += coefs[j + i * nteams + r * nteams * nteams];
         }


         /* create and add variable */
         SCIP_CALL( SCIPcreateVarBasic(scip, &var, NULL, 0.0, 1.0, matchingvalue, SCIP_VARTYPE_BINARY) );
         SCIPvarSetData(var, (SCIP_VARDATA*) (long) r);
         SCIP_CALL( SCIPvarSetInitial(var, TRUE) );
         SCIP_CALL( SCIPvarSetRemovable(var, TRUE) );
         SCIPvarMarkDeletable(var);
         SCIP_CALL( SCIPaddPricedVar(scip, var, 1.0) );
         SCIPdebugMsg(scip, "added variable %i\n", SCIPgetNVars(scip));
         /* catch variable deleted event on the variable to update the stablesetvars array in the problem data */
         SCIP_CALL( SCIPcatchVarEvent(scip, var, SCIP_EVENTTYPE_VARDELETED, SCIPfindEventhdlr(scip, EVENTHDLR_NAME),
            NULL, NULL) );

         /* set coefficient in round-constraint */
         SCIPgetTransformedCons(scip, conssmatchingonround[r], &cons);
         SCIP_CALL( SCIPaddCoefLinear(scip, cons, var, 1.0) );

         /* set coefficient in matching constraint */
         k = -1;
         for (i = 0; i < nteams; ++i)
         {
            for (j = i + 1; j < nteams; ++j)
            {
               ++k;
               /* edgeexists and weights arrays follow the natural unordered edge sorting,
                * that (i,j) < (i',j') if i < i', or i = i' and j < j'.
                */
               if ( solution[i] == j )
               {
                  SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchisplayed[k], &cons) );
                  SCIP_CALL( SCIPaddCoefLinear(scip, cons, var, 1.0) );
               }
            }
         }

         ++ncolsadded;
      }
   }

   SCIPfreeBufferArray(scip, &solution);
   SCIPfreeBufferArray(scip, &edgeexists);
   SCIPfreeBufferArray(scip, &weights);
   SCIPfreeBufferArray(scip, &conss);

   /* not finding a variable is also success */
   *result = SCIP_SUCCESS;

   return SCIP_OKAY;
}



/**@} */

/**name Callback methods
 *
 * @{
 */

/** destructor of variable pricer to free user data (called when SCIP is exiting) */
static
SCIP_DECL_PRICERFREE(pricerFreeSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/** initialization method of variable pricer (called after problem was transformed and pricer is active) */
static
SCIP_DECL_PRICERINIT(pricerInitSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/** deinitialization method of variable pricer (called before transformed problem is freed and pricer is active) */
static
SCIP_DECL_PRICEREXIT(pricerExitSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/** reduced cost pricing method of variable pricer for feasible LPs */
static
SCIP_DECL_PRICERREDCOST(pricerRedcostSRR)
{  /*lint --e{715}*/

   solvePricingSRR(scip, pricer, FALSE, result);

   return SCIP_OKAY;
}

/** farkas pricing method of variable pricer for infeasible LPs */
static
SCIP_DECL_PRICERFARKAS(pricerFarkasSRR)
{  /*lint --e{715}*/

   solvePricingSRR(scip, pricer, TRUE, result);

   return SCIP_OKAY;
}

/**@} */


/**@name Interface methods
 *
 * @{
 */

/** creates the SRR variable pricer and includes it in SCIP */
SCIP_RETCODE SCIPincludePricerSRR(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_PRICER* pricer;

   /* include variable pricer */
   SCIP_CALL( SCIPincludePricerBasic(scip, &pricer, PRICER_NAME, PRICER_DESC, PRICER_PRIORITY, PRICER_DELAY,
         pricerRedcostSRR, pricerFarkasSRR, NULL) );

   SCIP_CALL( SCIPsetPricerFree(scip, pricer, pricerFreeSRR) );
   SCIP_CALL( SCIPsetPricerInit(scip, pricer, pricerInitSRR) );
   SCIP_CALL( SCIPsetPricerExit(scip, pricer, pricerExitSRR) );

   /* include variable deleted event handler into SCIP */
   SCIP_CALL( SCIPincludeEventhdlrBasic(scip, NULL, EVENTHDLR_NAME, EVENTHDLR_DESC,
      eventExecVardeleted, NULL) );

   return SCIP_OKAY;
}

/** added problem specific data to pricer and activates pricer */
SCIP_RETCODE SCIPpricerSRRActivate(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_PRICER* pricer;

   assert(scip != NULL);

   pricer = SCIPfindPricer(scip, PRICER_NAME);
   assert(pricer != NULL);

   /* activate pricer */
   SCIP_CALL( SCIPactivatePricer(scip, pricer) );

   return SCIP_OKAY;
}

/**@} */
