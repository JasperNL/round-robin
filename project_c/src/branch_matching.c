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

/**@file   branch_matching.c
 * @brief  brancher for SRR
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include <assert.h>

#include "branch_matching.h"
#include "probdata_srr.h"
#include "cons_matchonround.h"


#define BRANCHRULE_NAME            "matchingbranch"
#define BRANCHRULE_DESC            "brancher for SRR"
#define BRANCHRULE_PRIORITY        10000000
#define BRANCHRULE_MAXDEPTH        -1
#define BRANCHRULE_MAXBOUNDDIST    1.0

/*
 * Local methods
 */

/** implements argsort
 *
 * The data pointer is a lookup array.
 */
static
SCIP_DECL_SORTINDCOMP(argsortDouble)
{
   SCIP_Real* args;
   args = (SCIP_Real*) dataptr;

   if( args[ind1] < args[ind2] )
      return -1;

   if( args[ind1] > args[ind2] )
      return 1;

   return 0;
}


/*
 * Callback methods of branching rule
 */

static
SCIP_RETCODE branchProbe(
   SCIP* scip,
   int i,
   int j,
   int r,
   SCIP_Bool direction,
   SCIP_Real* branchobj,
   SCIP_Bool* branchcutoff
)
{
   SCIP_NODE* newnode;
   SCIP_CONS* cons;
   SCIP_Bool cutoff;
   SCIP_Bool lperror;

   assert( !SCIPinProbing(scip) );
   /* start Probing */
   SCIP_CALL( SCIPstartProbing(scip) );
   /* create new probing node and add store graph cons to it with same(node1, node2) */
   SCIP_CALL( SCIPnewProbingNode(scip) );
   newnode = SCIPgetCurrentNode(scip);

   SCIPcreateConsSRRmor(scip, &cons, "probingcons", newnode, i, j, r, direction);
   SCIP_CALL( SCIPaddConsNode(scip, newnode, cons, NULL) );
   /* propagate the new b&b-node, i.e. fix vars to 0 that don't contain both node1 and node2 */
   SCIP_CALL( SCIPpropagateProbing(scip, -1, &cutoff, NULL) );
   /* solve the LP using pricing */
   SCIP_CALL( SCIPsolveProbingLPWithPricing(scip, FALSE, FALSE, -1, &lperror, &cutoff) );
   assert(!lperror);
   /* get the changed objective value */
   *branchobj = SCIPgetLPObjval(scip);
   *branchcutoff = cutoff;

   SCIP_CALL( SCIPdelCons(scip, cons) );
   SCIP_CALL( SCIPreleaseCons(scip, &cons) );
   SCIP_CALL( SCIPendProbing(scip) );

   return SCIP_OKAY;
}


/** branching execution method for fractional LP solutions */
static
SCIP_DECL_BRANCHEXECLP(branchExecsrr)
{  /*lint --e{715}*/
   int i;
   int j;
   int r;
   int k;
   int c;
   int nteams;
   int nrounds;
   int nmatches;
   SCIP_Real* coefs;
   SCIP_CONS** conssmatchingonround;
   SCIP_CONS** conssmatchisplayed;
   SCIP_CONS* cons;
   SCIP_Real* matchroundsol;
   SCIP_VAR* var;
   SCIP_ROW* row;
   SCIP_COL** cols;
   int ncols;
   SCIP_Real val;
   SCIP_Real maxval;
   SCIP_Real varval;
   SCIP_Real curobjval;
   int maxi;
   int maxj;
   int maxk;
   int maxr;
   SCIP_NODE* nodesame;
   SCIP_NODE* nodediff;

   SCIP_Real* scores;
   SCIP_Real score;
   int* bestscores;
   int l;
   int maxstrongbrnachingcandidates;
   SCIP_Real obj0;
   SCIP_Real obj1;
   SCIP_Bool cutoff;
   int ncutoff;
   SCIP_Bool solutionisintegral;
   SCIP_Bool modelisintegral;

   assert( scip != NULL );
   assert( branchrule != NULL );

   SCIP_CALL( SCIPgetProblemDataSRR(scip, &nteams, &nrounds, &coefs, &conssmatchingonround,
      &conssmatchisplayed) );

   nmatches = (nteams * (nteams - 1)) / 2;

   SCIPallocClearBufferArray(scip, &matchroundsol, nmatches * nrounds);

   /* Determine LP solution in node */
   k = -1;
   solutionisintegral = TRUE;
   modelisintegral = TRUE;
   for (i = 0; i < nteams; ++i)
   {
      for (j = i + 1; j < nteams; ++j)
      {
         ++k;
         assert( k < nmatches );

         SCIP_CALL( SCIPgetTransformedCons(scip, conssmatchisplayed[k], &cons) );
         row = SCIPconsGetRow(scip, cons);
         cols = SCIProwGetCols(row);
         ncols = SCIProwGetNNonz(row);

         for (c = 0; c < ncols; ++c)
         {
            /* we stored the round of the variable in its vardata */
            var = SCIPcolGetVar(cols[c]);
            r = (int) (long) SCIPvarGetData(var);
            varval = SCIPgetSolVal(scip, NULL, var);
            matchroundsol[k * nrounds + r] += varval;
            modelisintegral = modelisintegral && !SCIPisGT(scip, MIN(varval, 1.0 - varval), 0.0);
         }
         for (r = 0; r < nrounds; ++r)
         {
            val = matchroundsol[k * nrounds + r];
            solutionisintegral = solutionisintegral && !SCIPisGT(scip, MIN(val, 1.0 - val), 0.0);
         }
      }
   }
   assert( k == nmatches - 1 ); /* last index checked was last match */

   /* if the variables are such that an integral solution is attained */
   if ( solutionisintegral )
   {
      assert( modelisintegral );
      SCIPdebugMessage("Integral solution found. Stopping.\n");
      SCIPfreeBufferArray(scip, &matchroundsol);
      *result = SCIP_DIDNOTFIND;
      return modelisintegral ? SCIP_OKAY : SCIP_ERROR;
   }

   /* Most infeasible branching preselection */
   SCIP_CALL( SCIPallocBufferArray(scip, &scores, nmatches * nrounds) );
   SCIP_CALL( SCIPallocBufferArray(scip, &bestscores, nmatches * nrounds) );

   k = -1;
   for (i = 0; i < nteams; ++i)
   {
      for (j = i + 1; j < nteams; ++j)
      {
         ++k;
         assert( k < nmatches );
         assert( k == SCIPmatchGetIndex(nteams, i, j) );

         maxval = -1;
         for (r = 0; r < nrounds; ++r)
         {
            /* Prefer branching on one that has higher cost coefficients */
            val = matchroundsol[k * nrounds + r];
            val = MIN(val, 1.0 - val);

            if ( SCIPisGT(scip, val, 0.0) )
            {
               /* fractional variable */
               /* this looks a bit like x^2 * min(x, 1-x). */
               val = val /* want maximally fractional */
                  * (1.0 + ABS(coefs[i + j * nteams + r * nteams * nteams])) /* want variables with coefficient 1 */
                  * (matchroundsol[k * nrounds + r] * matchroundsol[k * nrounds + r]) /* i like high values */;
               scores[k * nrounds + r] = val;

               if ( val > maxval )
                  maxval = val;
            }
            else
            {
               /* integral variable */
               scores[k * nrounds + r] = -1;
            }
         }
         if ( maxval != -1 )
         {
            /* For non-integral guys, the best round gets a bonus */
            for (r = 0; r < nrounds; ++r)
            {
               if ( scores[k * nrounds + r] > maxval - 1e-6 )
                  scores[k * nrounds + r] += 0.2;
            }
         }
      }
   }
   assert( k == nmatches - 1 ); /* last index checked was last match */

   /* sort scores descendingly */
   SCIPsortDown(bestscores, argsortDouble, scores, nmatches * nrounds);

   /* strong branching, choose the best option */
   maxstrongbrnachingcandidates = (int) (((double) nmatches * nrounds) * .1 * pow(.65, SCIPgetDepth(scip)));
   if ( maxstrongbrnachingcandidates > nmatches * nrounds )
      maxstrongbrnachingcandidates = nmatches * nrounds;
   SCIPdebugMessage("Strong branching candidates at depth %d: %d\n", SCIPgetDepth(scip), maxstrongbrnachingcandidates);

   /* if there is only 1 strong branching candidate, then this is the one we go for. No strong branching. */
   if ( maxstrongbrnachingcandidates <= 1 )
   {
      k = bestscores[0];

      /* integral solution cannot be found */
      assert( scores[k] > -1 );

      r = k % nrounds;
      k /= nrounds;
      SCIPmatchIndexGetTeams(nteams, k, &i, &j);
      maxi = i;
      maxj = j;
      maxk = k;
      maxr = r;
      maxval = -1;
      goto FOUNDBRANCHINGCANDIDATE;
   }

   curobjval = SCIPgetLPObjval(scip);
   maxi = -1;
   maxj = -1;
   maxk = -1;
   maxr = -1;
   maxval = -1;
   for (l = 0; l < maxstrongbrnachingcandidates; ++l)
   {
      k = bestscores[l];
      score = scores[k];
      /* score is -1 if that was integral */
      if ( score == -1 )
         break;

      /* get the match and round */
      r = k % nrounds;
      k /= nrounds;
      SCIPmatchIndexGetTeams(nteams, k, &i, &j);

#ifndef NDEBUG
      /* must be integral */
      val = matchroundsol[k * nrounds + r];
      val = MIN(val, 1.0 - val);
      assert( SCIPisGT(scip, val, 0.0) );
#endif

      val = matchroundsol[k * nrounds + r];

      ncutoff = 0;
      SCIP_CALL( branchProbe(scip, i, j, r, FALSE, &obj0, &cutoff) );
      // SCIPdebugMessage("%3d %3d %3d (%lf) -> %lf (%s) ", i, j, r, MIN(val, 1.0 - val), obj0, cutoff?"!":" ");
      if ( cutoff ) ++ncutoff;

      SCIP_CALL( branchProbe(scip, i, j, r, TRUE, &obj1, &cutoff) );
      // SCIPdebugMessage("%lf (%s)\n", obj1, cutoff?"!":" ");
      if ( cutoff ) ++ncutoff;

      if ( ncutoff > 0 )
      {
         maxi = i;
         maxj = j;
         maxk = k;
         maxr = r;
         maxval = INFINITY;  /* we like cutoffs a lot! */
         goto FOUNDBRANCHINGCANDIDATE;
      }

      if ( (obj0 - curobjval + 0.1) * (obj1 - curobjval + 0.1) > maxval )
      {
         maxi = i;
         maxj = j;
         maxk = k;
         maxr = r;
         maxval = (obj0 - curobjval + 0.1) * (obj1 - curobjval + 0.1);
         SCIPdebugMessage("Candidate %d looks good. (%d,%d) at round %d, coeff %lf, value %lf, objective {%lf, %lf}"
            " for LP-obj %lf.\n", l, i, j, r, coefs[i + j * nteams + r * nteams * nteams],
            matchroundsol[k * nrounds + r], obj0, obj1, curobjval);
      }
   }

   if ( maxval != -1 )
      goto FOUNDBRANCHINGCANDIDATE;

   FOUNDBRANCHINGCANDIDATE:
   SCIPfreeBufferArray(scip, &bestscores);
   SCIPfreeBufferArray(scip, &scores);
   SCIPdebugMessage("Branching on %d %d %d\n", maxi, maxj, maxr);

#ifndef NDEBUG
   /* The branching choice must be fractional. */
   val = matchroundsol[maxk * nrounds + maxr];
   val = MIN(val, 1.0 - val);
   assert( SCIPisGT(scip, val, 0.0) );
#endif

   SCIPmatchIndexGetTeams(nteams, maxk, &i, &j);
   SCIPdebugMessage("Match (i, j) = (%d,%d) (k=%d) on round r=%d has value %lf\n",
      i, j, maxk, maxr, matchroundsol[maxk * nrounds + maxr]);
   assert( i == maxi );
   assert( j == maxj );

   /* No integral solution is found. Branch on match {maxi, maxj} on round r. */
   SCIP_CALL( SCIPcreateChild(scip, &nodesame,
      1.0 + coefs[maxj + maxi * nteams + maxr * nteams * nteams] /* higher coefficient is higher branching priority */,
      SCIPnodeGetEstimate(SCIPgetCurrentNode(scip)) + /* Current objective */
      (1.0 - matchroundsol[maxk * nrounds + maxr]) * coefs[maxj + maxi * nteams + maxr * nteams * nteams]
      /* add marginal difference */
   ) );

   assert( allowaddcons );
   SCIP_CALL( SCIPcreateConsSRRmor(scip, &cons, "", nodesame, maxi, maxj, maxr, TRUE) );
   SCIP_CALL( SCIPaddConsNode(scip, nodesame, cons, NULL) );


   /* Do not remove marignal difference, since we know it's optimal like this */
   SCIP_CALL( SCIPcreateChild(scip, &nodediff, 0.0,
      SCIPnodeGetEstimate(SCIPgetCurrentNode(scip)) /* Current objective */
   ) );

   SCIP_CALL( SCIPcreateConsSRRmor(scip, &cons, "", nodediff, maxi, maxj, maxr, FALSE) );
   SCIP_CALL( SCIPaddConsNode(scip, nodediff, cons, NULL) );

   *result = SCIP_BRANCHED;
   SCIPfreeBufferArray(scip, &matchroundsol);
   return SCIP_OKAY;
}

/*
 * branching rule specific interface methods
 */

/** creates the mutlinode branching rule and includes it in SCIP */
SCIP_RETCODE SCIPincludeBranchruleSRR(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_BRANCHRULEDATA* branchruledata;
   SCIP_BRANCHRULE* branchrule;

   branchruledata = NULL;

   /* include branching rule */
   /* use SCIPincludeBranchruleBasic() plus setter functions if you want to set callbacks one-by-one
    * and your code should compile independent of new callbacks being added in future SCIP versions
    */
   SCIP_CALL( SCIPincludeBranchruleBasic(scip, &branchrule, BRANCHRULE_NAME, BRANCHRULE_DESC, BRANCHRULE_PRIORITY,
      BRANCHRULE_MAXDEPTH, BRANCHRULE_MAXBOUNDDIST, branchruledata) );

   assert(branchrule != NULL);

   /* set non fundamental callbacks via setter functions */
   SCIP_CALL( SCIPsetBranchruleExecLp(scip, branchrule, branchExecsrr) );

   return SCIP_OKAY;
}
