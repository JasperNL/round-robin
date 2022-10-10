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

/**@file   probdata_srr.c
 * @brief  problem data for SRR
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/
#include "probdata_srr.h"

#include "scip/cons_nonlinear.h"
#include "scip/cons_linear.h"
#include "scip/cons_logicor.h"
#include "scip/var.h"
#include "pricer_srr.h"
#include "cons_matchonround.h"
#include "scip/cons_linear.h"
#include <assert.h>

#include "scip/struct_cons.h"
#include "scip/struct_lp.h"
#include "scip/struct_mem.h"
#include "scip/struct_primal.h"
#include "scip/struct_prob.h"
#include "scip/struct_scip.h"
#include "scip/struct_set.h"
#include "scip/struct_stat.h"
#include "scip/struct_var.h"

struct SCIP_ProbData
{
   int nteams;
   int nrounds;
   SCIP_Real* coefs;
   SCIP_CONS** conssmatchingonround;
   SCIP_CONS** conssmatchisplayed;
};
// typedef struct SCIP_ProbData SCIP_PROBDATA;


/** Get the index of the match.
 * The matches are ordered as follows
 * 01 02 03 04 05 06 07
 * 12 13 14 15 16 17
 * 23 24 25 26 27
 * 34 35 36 37
 * 45 46 47
 * 56 57
 * 67
 * so for match ij (i.e., {i, j} with i < j),
 * the index is
 * (j - i) + \sum_{k=1}^{i} (nteams - 1 - k) = (j - i) + ((i * (2*nteams - i - 1)) / 2)
 * Minus one, since indexing starts at zero.
 */
int SCIPmatchGetIndex(
   int nteams,
   int i,
   int j
)
{
   assert( i < j );
   return (j - i) + ((i * (2*nteams - i - 1)) / 2) - 1;
}


void SCIPmatchIndexGetTeams(
   int nteams,
   int k,
   int* i,
   int* j
)
{
   int l;
#ifndef NDEBUG
   int origk = k;
#endif
   *i = 0;
   for (l = nteams - 1; l > 0; --l)
   {
      if (k + *i + 1 < nteams)
         break;
      ++*i;
      k -= l;
   }
   *j = k + *i + 1;
   assert( *i >= 0 && *i < nteams );
   assert( *j >= 0 && *j < nteams );
   assert( *i < *j );
   assert( SCIPmatchGetIndex(nteams, *i, *j) == origk );
}


/** get problem data (just naively unpack the struct) */
SCIP_RETCODE SCIPgetProblemDataSRR(
   SCIP* scip,
   int* nteams,
   int* nrounds,
   SCIP_Real** coefs,
   SCIP_CONS*** conssmatchingonround,
   SCIP_CONS*** conssmatchisplayed
)
{
   SCIP_PROBDATA* probdata;

   assert( scip != NULL );
   assert( nteams != NULL );
   assert( nrounds != NULL );
   assert( coefs != NULL );
   assert( conssmatchingonround != NULL );
   assert( conssmatchisplayed != NULL );

   probdata = SCIPgetProbData(scip);
   assert( probdata != NULL );

   *nteams = probdata->nteams;
   *nrounds = probdata->nrounds;
   *coefs = probdata->coefs;
   *conssmatchingonround = probdata->conssmatchingonround;
   *conssmatchisplayed = probdata->conssmatchisplayed;

   return SCIP_OKAY;
}


/** transforms the problem */
static
SCIP_DECL_PROBTRANS(probtransSRR)
{
   *targetdata = sourcedata;
   return SCIP_OKAY;
}


/** deletes the transformed problem */
static
SCIP_DECL_PROBDELTRANS(probdeltransSRR)
{
   int i;
   SCIP_VAR** vars;
   SCIP_VAR* var;
   int nvars;

   SCIP_CONSHDLR* conshdlr;
   SCIP_CONSHDLR** conshdlrs;
   int nconshdlrs;
   SCIP_CONS** conss;
   SCIP_CONS** consscopy;
   int nconss;

   assert(scip != NULL);
   assert(probdata != NULL);

   /* release constraints */
   conshdlrs = SCIPgetConshdlrs(scip);
   nconshdlrs = SCIPgetNConshdlrs(scip);
   for (i = 0; i < nconshdlrs; ++i)
   {
      conshdlr = conshdlrs[i];
      if ( strcmp(SCIPconshdlrGetName(conshdlr), "srrmor") >= 0 )
         break;
   }
   assert( i < nconshdlrs );

   conss = SCIPconshdlrGetConss(conshdlr);
   nconss = SCIPconshdlrGetNConss(conshdlr);
   if ( nconss > 0 )
   {
      assert( conss != NULL );
      SCIP_CALL( SCIPduplicateBlockMemoryArray(scip, &consscopy, conss, nconss) );
      for (i = nconss - 1; i >=0; --i)
      {
         SCIP_CALL( SCIPreleaseCons(scip, &consscopy[i]) );
      }
      SCIPfreeBlockMemoryArray(scip, &conss, nconss);
   }

   /* release variables */
   vars = scip->transprob->vars;
   nvars = scip->transprob->nvars;

   for (i = 0; i < nvars; ++i)
   {
      var = vars[i];
      assert( ! SCIPvarIsDeleted(var) );
      SCIPreleaseVar(scip, &var);
   }

   return SCIP_OKAY;
}

static
SCIP_DECL_PROBDELORIG(probdelorigSRR)
{
   int nteams;
   int nrounds;
   int nmatches;
   int nelem;
   int i;

   assert( scip != NULL );
   assert( probdata != NULL );
   assert( *probdata != NULL );

   nteams = (*probdata)->nteams;
   nrounds = (*probdata)->nrounds;
   nmatches = (nteams / 2) * (nteams - 1);
   nelem = nteams * nteams * nrounds;

   for (i = 0; i < nmatches; ++i)
   {
      SCIP_CALL( SCIPreleaseCons(scip, &((*probdata)->conssmatchisplayed[i])) );
   }

   for (i = 0; i < nrounds; ++i)
   {
      SCIP_CALL( SCIPreleaseCons(scip, &((*probdata)->conssmatchingonround[i])) );
   }

   SCIPfreeBlockMemoryArray(scip, &((*probdata)->conssmatchisplayed), nmatches);
   SCIPfreeBlockMemoryArray(scip, &((*probdata)->conssmatchingonround), nrounds);
   SCIPfreeBlockMemoryArray(scip, &((*probdata)->coefs), nelem);

   /* free probdata */
   SCIPfreeBlockMemory(scip, probdata);

   return SCIP_OKAY;
}


/** create the probdata for a SRR problem */
SCIP_RETCODE SCIPcreateProbSRR(
   SCIP*                 scip,               /**< SCIP data structure */
   const char*           name,               /**< problem name */
   int                   nteams,             /**< number of teams */
   int                   nrounds,            /**< number of rounds, must be nteams - 1 */
   SCIP_Real*            coefs               /**< objective coefficients */
)
{
   int k;
   int nmatches;
   int nelem;
   SCIP_Real coeffrac;
   SCIP_Real coefint;
   SCIP_PROBDATA* probdata;
   SCIP_CONS** conssmatchingonround;
   SCIP_CONS** conssmatchisplayed;

   assert( scip != NULL );
   assert( nteams > 0 );
   assert( nteams % 2 == 0 );
   assert( nrounds == nteams - 1 );
   assert( coefs != NULL );

   nmatches = (nteams / 2) * (nteams - 1);
   nelem = nteams * nteams * nrounds;

   SCIP_CALL( SCIPallocBlockMemory(scip, &probdata) );
   probdata->nteams = nteams;
   probdata->nrounds = nrounds;
   SCIP_CALL( SCIPduplicateBlockMemoryArray(scip, &(probdata->coefs), coefs, nelem) );

   SCIP_CALL( SCIPcreateProb(scip, name, probdelorigSRR, probtransSRR, probdeltransSRR,
      NULL, NULL, NULL, probdata) );

   SCIP_CALL( SCIPsetBoolParam(scip, "pricing/delvars", TRUE) );
   SCIP_CALL( SCIPsetBoolParam(scip, "pricing/delvarsroot", TRUE) );
   SCIP_CALL( SCIPsetBoolParam(scip, "lp/cleanupcolsroot", TRUE) );
   SCIP_CALL( SCIPsetBoolParam(scip, "lp/cleanupcols", TRUE) );
   SCIP_CALL( SCIPsetIntParam(scip, "lp/colagelimit", 2) );

   /* integral objective if coefficients are all integral */
   for (k = 0; k < nelem; ++k)
   {
      coeffrac = modf(coefs[k], &coefint);
      if ( SCIPisGT(scip, MIN(coeffrac, 1.0 - coeffrac), 0.0) )
         break;
   }
   if ( k == nelem )
   {
      /* loop is not broken, integral is objective */
      SCIPinfoMessage(scip, NULL, "Objective is integral\n");
      SCIP_CALL( SCIPsetObjIntegral(scip) );
   }

   SCIP_CALL( SCIPpricerSRRActivate(scip) );

   /* linear constraint: a matching on each round */
   SCIPallocBlockMemoryArray(scip, &conssmatchingonround, nrounds);
   for (k = 0; k < nrounds; ++k)
   {
      char ctrname[128];
      snprintf(ctrname, sizeof(ctrname), "round[%d]", k);
      SCIP_CALL( SCIPcreateConsLinear(scip, &(conssmatchingonround[k]), ctrname, 0, NULL, NULL, 1.0, 1.0, TRUE, TRUE,
         TRUE, TRUE, TRUE, FALSE, TRUE /*modifiable*/, FALSE, FALSE, FALSE) );
      SCIPaddCons(scip, conssmatchingonround[k]);
   }
   probdata->conssmatchingonround = conssmatchingonround;

   /* linear constraint: every match is played */
   SCIPallocBlockMemoryArray(scip, &conssmatchisplayed, nmatches);
   for (k = 0; k < nmatches; ++k)
   {
      char ctrname[128];
      snprintf(ctrname, sizeof(ctrname), "match[%d]", k);
      SCIP_CALL( SCIPcreateConsLinear(scip, &(conssmatchisplayed[k]), ctrname, 0, NULL, NULL, 1.0, 1.0, TRUE, TRUE,
         TRUE, TRUE, TRUE, FALSE, TRUE /*modifiable*/, FALSE, FALSE, FALSE) );
      SCIPaddCons(scip, conssmatchisplayed[k]);
   }
   probdata->conssmatchisplayed = conssmatchisplayed;

   return SCIP_OKAY;
}
