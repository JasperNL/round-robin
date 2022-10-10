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

/**@file   cons_SRR.c
 * @brief  constraint handler for enforcing matches on round
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "scip/scip.h"
#include "cons_matchonround.h"
#include "probdata_srr.h"


/* fundamental constraint handler properties */
#define CONSHDLR_NAME          "srrmor"
#define CONSHDLR_DESC          "SRR match on round constraint handler"
#define CONSHDLR_ENFOPRIORITY         1 /**< priority of the constraint handler for constraint enforcing */
#define CONSHDLR_CHECKPRIORITY       -1 /**< priority of the constraint handler for checking feasibility */
#define CONSHDLR_EAGERFREQ            1 /**< frequency for using all instead of only the useful constraints in separation,
                                         *   propagation and enforcement, -1 for no eager evaluations, 0 for first only */
#define CONSHDLR_NEEDSCONS        FALSE /**< should the constraint handler be skipped, if no constraints are available? */

/* optional constraint handler properties */
/* TODO: remove properties which are never used because the corresponding routines are not supported */
#define CONSHDLR_SEPAPRIORITY         0 /**< priority of the constraint handler for separation */
#define CONSHDLR_SEPAFREQ            -1 /**< frequency for separating cuts; zero means to separate only in the root node */
#define CONSHDLR_DELAYSEPA        FALSE /**< should separation method be delayed, if other separators found cuts? */

#define CONSHDLR_PROPFREQ             1 /**< frequency for propagating domains; zero means only preprocessing propagation */
#define CONSHDLR_DELAYPROP        FALSE /**< should propagation method be delayed, if other propagators found reductions? */
#define CONSHDLR_PROP_TIMING     SCIP_PROPTIMING_BEFORELP /**< propagation timing mask of the constraint handler*/

#define CONSHDLR_PRESOLTIMING    SCIP_PRESOLTIMING_MEDIUM /**< presolving timing of the constraint handler (fast, medium, or exhaustive) */
#define CONSHDLR_MAXPREROUNDS        -1 /**< maximal number of presolving rounds the constraint handler participates in (-1: no limit) */


/*
 * Data structures
 */

struct SCIP_ConsData
{
   int i;
   int j;
   int r;
   SCIP_Bool permitted;
};

/** constraint handler data */

/*
 * Local methods
 */

SCIP_RETCODE SCIPgetConstraintDataSRRmor(
   SCIP_CONS* cons,
   int* i,
   int* j,
   int* r,
   SCIP_Bool* permitted
)
{
   SCIP_CONSDATA* consdata;

   assert( cons != NULL );
   consdata = SCIPconsGetData(cons);

   *i = consdata->i;
   *j = consdata->j;
   *r = consdata->r;
   *permitted = consdata->permitted;

   return SCIP_OKAY;
}


/*
 * Callback methods of constraint handler
 */


/** destructor of constraint handler to free constraint handler data (called when SCIP is exiting) */
static
SCIP_DECL_CONSFREE(consFreeSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}


/** frees specific constraint data */
static
SCIP_DECL_CONSDELETE(consDeleteSRR)
{  /*lint --e{715}*/

   assert(conshdlr != NULL);
   assert(strcmp(SCIPconshdlrGetName(conshdlr), CONSHDLR_NAME) == 0);
   assert( cons != NULL );
   assert( consdata != NULL );
   assert( *consdata != NULL );
   assert( (*consdata)->i >= 0 );
   assert( (*consdata)->j >= 0 );
   assert( (*consdata)->r >= 0 );
   assert( (*consdata)->permitted == TRUE || (*consdata)->permitted == FALSE );

   SCIPfreeBlockMemory(scip, consdata);

   return SCIP_OKAY;
}


/** constraint enforcing method of constraint handler for LP solutions */
static
SCIP_DECL_CONSENFOLP(consEnfolpSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}


/** constraint enforcing method of constraint handler for pseudo solutions */
static
SCIP_DECL_CONSENFOPS(consEnfopsSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}


/** feasibility check method of constraint handler for integral solutions */
static
SCIP_DECL_CONSCHECK(consCheckSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/** variable rounding lock method of constraint handler */
static
SCIP_DECL_CONSLOCK(consLockSRR)
{  /*lint --e{715}*/
   return SCIP_OKAY;
}

/** domain propagation method of constraint handler */
static
SCIP_DECL_CONSPROP(consPropSRR)
{
   int nteams;
   int nrounds;
   SCIP_Real* coefs;
   SCIP_CONS** conssmatchingonround;
   SCIP_CONS** conssmatchisplayed;

   int cid;
   SCIP_CONS* cons;
   SCIP_CONSDATA* consdata;
   int c;
   int i;
   int j;
   int k;
   int r;
   SCIP_ROW* row;
   SCIP_COL** cols;
   int ncols;
   SCIP_VAR* var;

   SCIP_CALL( SCIPgetProblemDataSRR(scip, &nteams, &nrounds, &coefs, &conssmatchingonround,
      &conssmatchisplayed) );

   assert( scip != NULL );
   assert( conss != NULL || nconss <= 0 );
   assert( nconss >= 0 );

   *result = SCIP_DIDNOTFIND;

   for(cid = 0; cid < nconss; ++cid)
   {
      /* @todo! */
      cons = conss[cid];
      assert( cons != NULL );

      consdata = SCIPconsGetData(cons);
      assert( consdata != NULL );
      assert( consdata->i < consdata->j );

      if ( consdata->permitted )
      {
         /* team i must play against team j in round r. */
         k = -1;
         for (i = 0; i < nteams; ++i)
         {
            for (j = i + 1; j < nteams; ++j)
            {
               ++k;

               SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchisplayed[k], &cons) );
               row = SCIPconsGetRow(scip, cons);
               cols = SCIProwGetCols(row);
               ncols = SCIProwGetNNonz(row);

               if ( i == consdata->i && j == consdata->j )
               {
                  /* i and j play against each other.
                   * On all rounds other than round consdata->r, the y-variables must be disabled. */
                  for (c = 0; c < ncols; ++c)
                  {
                     /* we stored the round of the variable in its vardata */
                     var = SCIPcolGetVar(cols[c]);
                     r = (int) (long) SCIPvarGetData(var);
                     if ( r != consdata->r )
                     {
                        if ( SCIPisGT(scip, SCIPvarGetUbLocal(var), 0.0) )
                        {
                           *result = SCIP_REDUCEDDOM;
                           SCIPchgVarUb(scip, var, 0.0);
                        }
                     }
                  }
               }
               else if ( i == consdata->i || j == consdata->j )
               {
                  /* i and j have a different opponent in this matching. Disable y-variables on round r. */
                  for (c = 0; c < ncols; ++c)
                  {
                     /* we stored the round of the variable in its vardata */
                     var = SCIPcolGetVar(cols[c]);
                     r = (int) (long) SCIPvarGetData(var);
                     if ( r == consdata->r )
                     {
                        if ( SCIPisGT(scip, SCIPvarGetUbLocal(var), 0.0) )
                        {
                           *result = SCIP_REDUCEDDOM;
                           SCIPchgVarUb(scip, var, 0.0);
                        }
                     }
                  }
               }
            }
         }
      }
      else
      {
         /* team i must not play against team j in round r. */
         k = -1;
         for (i = 0; i < nteams; ++i)
         {
            for (j = i + 1; j < nteams; ++j)
            {
               ++k;

               SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchisplayed[k], &cons) );
               row = SCIPconsGetRow(scip, cons);
               cols = SCIProwGetCols(row);
               ncols = SCIProwGetNNonz(row);

               if ( i == consdata->i && j == consdata->j )
               {
                  /* i and j play against each other.
                   * On round r, these y-variables must be disabled. */
                  for (c = 0; c < ncols; ++c)
                  {
                     /* we stored the round of the variable in its vardata */
                     var = SCIPcolGetVar(cols[c]);
                     r = (int) (long) SCIPvarGetData(var);
                     if ( r == consdata->r )
                     {
                        if ( SCIPisGT(scip, SCIPvarGetUbLocal(var), 0.0) )
                        {
                           SCIPchgVarUb(scip, var, 0.0);
                           *result = SCIP_REDUCEDDOM;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return SCIP_OKAY;
}/*lint !e715*/


/*
 * constraint specific interface methods
 */

/** creates and captures a SRR MOR constraint */
SCIP_RETCODE SCIPcreateConsSRRmor(
   SCIP*                 scip,               /**< SCIP data structure */
   SCIP_CONS**           cons,               /**< pointer to hold the created constraint */
   const char*           name,               /**< name of constraint */
   SCIP_NODE*            stickingnode,       /**< the B&B-tree node at which the constraint will be sticking */
   int                   teami,              /**< first team */
   int                   teamj,              /**< second team */
   int                   round,              /**< round of constraint */
   SCIP_Bool             permitted           /**< whether match {i, j} is permitted on round `round` or not. */
)
{
   SCIP_CONSHDLR* conshdlr;
   SCIP_CONSDATA* consdata;

   assert(scip != NULL);
   assert(stickingnode != NULL);

   /* find the storeGraph constraint handler */
   conshdlr = SCIPfindConshdlr(scip, CONSHDLR_NAME);
   if ( conshdlr == NULL )
   {
      SCIPerrorMessage("SRRMOR constraint handler not found\n");
      return SCIP_PLUGINNOTFOUND;
   }

   /* create constraint data */
   SCIP_CALL( SCIPallocBlockMemory(scip, &consdata) );

   consdata->i = teami;
   consdata->j = teamj;
   consdata->r = round;
   consdata->permitted = permitted;

   /* create constraint */
   SCIP_CALL( SCIPcreateCons(scip, cons, name, conshdlr, consdata, FALSE, FALSE, FALSE, FALSE, TRUE,
         TRUE, FALSE, TRUE, FALSE, TRUE) );

   return SCIP_OKAY;
}

/** creates the handler for SRR */
SCIP_RETCODE SCIPincludeConshdlrSRR(
   SCIP*                 scip                /**< SCIP data structure */
)
{
   SCIP_CONSHDLR* conshdlr;

   /* include constraint handler */
   SCIP_CALL( SCIPincludeConshdlrBasic(scip, &conshdlr, CONSHDLR_NAME, CONSHDLR_DESC,
         CONSHDLR_ENFOPRIORITY, CONSHDLR_CHECKPRIORITY, CONSHDLR_EAGERFREQ, CONSHDLR_NEEDSCONS,
         consEnfolpSRR, consEnfopsSRR, consCheckSRR, consLockSRR, NULL) );
   assert(conshdlr != NULL);

   /* set non-fundamental callbacks via specific setter functions */
   SCIP_CALL( SCIPsetConshdlrProp(scip, conshdlr, consPropSRR, CONSHDLR_PROPFREQ, CONSHDLR_DELAYPROP,
      CONSHDLR_PROP_TIMING) );
   SCIP_CALL( SCIPsetConshdlrFree(scip, conshdlr, consFreeSRR) );
   SCIP_CALL( SCIPsetConshdlrDelete(scip, conshdlr, consDeleteSRR) );

   return SCIP_OKAY;
}
