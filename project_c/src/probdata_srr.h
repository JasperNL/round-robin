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

/**@file   probdata_srr.h
 * @brief  problem data for SRR
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "scip/scip.h"
#include "tclique/tclique.h"
#include "scip/cons_setppc.h"
#include "scip/type_cons.h"
#include "scip/def.h"


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
);

void SCIPmatchIndexGetTeams(
   int nteams,
   int k,
   int* i,
   int* j
);

/** get problem data (just naively unpack the struct) */
SCIP_RETCODE SCIPgetProblemDataSRR(
   SCIP* scip,
   int* nteams,
   int* nrounds,
   SCIP_Real** coefs,
   SCIP_CONS*** conssmatchingonround,
   SCIP_CONS*** conssmatchisplayed
);


/** create the probdata for a SRR problem */
SCIP_RETCODE SCIPcreateProbSRR(
   SCIP*                 scip,               /**< SCIP data structure */
   const char*           name,               /**< problem name */
   int                   nteams,             /**< number of teams */
   int                   nrounds,            /**< number of rounds, must be nteams - 1 */
   SCIP_Real*            coefs               /**< objective coefficients */
);
