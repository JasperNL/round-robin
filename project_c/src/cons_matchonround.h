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


SCIP_RETCODE SCIPgetConstraintDataSRRmor(
   SCIP_CONS* cons,
   int* i,
   int* j,
   int* r,
   SCIP_Bool* permitted
);

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
);


/** creates the handler for SRR */
SCIP_RETCODE SCIPincludeConshdlrSRR(
   SCIP*                 scip                /**< SCIP data structure */
);

