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

/**@file   pricer_srr.h
 * @brief  SRR variable pricer
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#ifndef __BIN_PRICER_RINGPACKING__
#define __BIN_PRICER_RINGPACKING__

#include "scip/scip.h"


/** creates the SRR variable pricer and includes it in SCIP */
SCIP_RETCODE SCIPincludePricerSRR(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** added problem specific data to pricer and activates pricer */
SCIP_RETCODE SCIPpricerSRRActivate(
   SCIP*                 scip                /**< SCIP data structure */
   );

#endif
