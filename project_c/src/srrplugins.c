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

/**@file   srrplugins.c
 * @brief  SCIP plugins for SRR
 * @author Jasper van Doornmalen
 */

/*--+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/

#include "srrplugins.h"

#include "scip/scipdefplugins.h"
#include "branch_matching.h"
#include "reader_srr.h"
#include "pricer_srr.h"
#include "cons_matchonround.h"

/** includes default plugins for SRR into SCIP */
SCIP_RETCODE SCIPincludeSRRPlugins(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   /* Include all default plugins except for the readers (only .srr), all separators and diving heuristics */

   /* include some default dialogs, since other plugins require that at least the root dialog is available */
   SCIP_CALL( SCIPincludeDialogDefaultBasic(scip) );

   SCIP_CALL( SCIPincludeConshdlrNonlinear(scip) ); /* nonlinear constraint handler must be before linear due to constraint upgrading */
   SCIP_CALL( SCIPincludeConshdlrLinear(scip) ); /* linear must be before its specializations due to constraint upgrading */

   SCIP_CALL( SCIPincludePresolBoundshift(scip) );
   SCIP_CALL( SCIPincludePresolConvertinttobin(scip) );
   SCIP_CALL( SCIPincludePresolDomcol(scip) );
   SCIP_CALL( SCIPincludePresolDualagg(scip) );
   SCIP_CALL( SCIPincludePresolDualcomp(scip) );
   SCIP_CALL( SCIPincludePresolDualinfer(scip) );
   SCIP_CALL( SCIPincludePresolGateextraction(scip) );
   SCIP_CALL( SCIPincludePresolImplics(scip) );
   SCIP_CALL( SCIPincludePresolInttobinary(scip) );
#ifdef SCIP_WITH_PAPILO
   SCIP_CALL( SCIPincludePresolMILP(scip) );
#endif
   SCIP_CALL( SCIPincludePresolQPKKTref(scip) );
   SCIP_CALL( SCIPincludePresolRedvub(scip) );
   SCIP_CALL( SCIPincludePresolTrivial(scip) );
   SCIP_CALL( SCIPincludePresolTworowbnd(scip) );
   SCIP_CALL( SCIPincludePresolSparsify(scip) );
   SCIP_CALL( SCIPincludePresolDualsparsify(scip) );
   SCIP_CALL( SCIPincludePresolStuffing(scip) );
   SCIP_CALL( SCIPincludeNodeselBfs(scip) );
   SCIP_CALL( SCIPincludeNodeselBreadthfirst(scip) );
   SCIP_CALL( SCIPincludeNodeselDfs(scip) );
   SCIP_CALL( SCIPincludeNodeselEstimate(scip) );
   SCIP_CALL( SCIPincludeNodeselHybridestim(scip) );
   SCIP_CALL( SCIPincludeNodeselRestartdfs(scip) );
   SCIP_CALL( SCIPincludeNodeselUct(scip) );

   SCIP_CALL( SCIPincludeConshdlrIndicator(scip) );
   SCIP_CALL( SCIPincludeConshdlrIntegral(scip) );

   SCIP_CALL( SCIPincludeEventHdlrEstim(scip) );
   SCIP_CALL( SCIPincludeEventHdlrSolvingphase(scip) );
   SCIP_CALL( SCIPincludeComprLargestrepr(scip) );
   SCIP_CALL( SCIPincludeComprWeakcompr(scip) );
   SCIP_CALL( SCIPincludeHeurBound(scip) );
   SCIP_CALL( SCIPincludeHeurClique(scip) );
   SCIP_CALL( SCIPincludeHeurCoefdiving(scip) );
   SCIP_CALL( SCIPincludeHeurCompletesol(scip) );
   SCIP_CALL( SCIPincludeHeurCrossover(scip) );
   SCIP_CALL( SCIPincludeHeurDins(scip) );
   SCIP_CALL( SCIPincludeHeurDps(scip) );
   SCIP_CALL( SCIPincludeHeurDualval(scip) );
   SCIP_CALL( SCIPincludeHeurFeaspump(scip) );
   SCIP_CALL( SCIPincludeHeurFixandinfer(scip) );
   SCIP_CALL( SCIPincludeHeurGins(scip) );
   SCIP_CALL( SCIPincludeHeurZeroobj(scip) );
   SCIP_CALL( SCIPincludeHeurIndicator(scip) );
   SCIP_CALL( SCIPincludeHeurIntshifting(scip) );
   SCIP_CALL( SCIPincludeHeurLocalbranching(scip) );
   SCIP_CALL( SCIPincludeHeurLocks(scip) );
   SCIP_CALL( SCIPincludeHeurLpface(scip) );
   SCIP_CALL( SCIPincludeHeurAlns(scip) );
   SCIP_CALL( SCIPincludeHeurNlpdiving(scip) );
   SCIP_CALL( SCIPincludeHeurMutation(scip) );
   SCIP_CALL( SCIPincludeHeurMultistart(scip) );
   SCIP_CALL( SCIPincludeHeurMpec(scip) );
   SCIP_CALL( SCIPincludeHeurOctane(scip) );
   SCIP_CALL( SCIPincludeHeurOfins(scip) );
   SCIP_CALL( SCIPincludeHeurOneopt(scip) );
   SCIP_CALL( SCIPincludeHeurPADM(scip) );
   SCIP_CALL( SCIPincludeHeurProximity(scip) );
   SCIP_CALL( SCIPincludeHeurRandrounding(scip) );
   SCIP_CALL( SCIPincludeHeurRens(scip) );
   SCIP_CALL( SCIPincludeHeurReoptsols(scip) );
   SCIP_CALL( SCIPincludeHeurRepair(scip) );
   SCIP_CALL( SCIPincludeHeurRins(scip) );
   SCIP_CALL( SCIPincludeHeurRounding(scip) );
   SCIP_CALL( SCIPincludeHeurShiftandpropagate(scip) );
   SCIP_CALL( SCIPincludeHeurShifting(scip) );
   SCIP_CALL( SCIPincludeHeurSimplerounding(scip) );
   SCIP_CALL( SCIPincludeHeurSubNlp(scip) );
   SCIP_CALL( SCIPincludeHeurTrivial(scip) );
   SCIP_CALL( SCIPincludeHeurTrivialnegation(scip) );
   SCIP_CALL( SCIPincludeHeurTrustregion(scip) );
   SCIP_CALL( SCIPincludeHeurTrySol(scip) );
   SCIP_CALL( SCIPincludeHeurTwoopt(scip) );
   SCIP_CALL( SCIPincludeHeurUndercover(scip) );
   SCIP_CALL( SCIPincludeHeurVbounds(scip) );
   SCIP_CALL( SCIPincludeHeurZirounding(scip) );
   SCIP_CALL( SCIPincludePropDualfix(scip) );
   SCIP_CALL( SCIPincludePropGenvbounds(scip) );
   SCIP_CALL( SCIPincludePropObbt(scip) );
   SCIP_CALL( SCIPincludePropNlobbt(scip) );
   SCIP_CALL( SCIPincludePropProbing(scip) );
   SCIP_CALL( SCIPincludePropPseudoobj(scip) );
   SCIP_CALL( SCIPincludePropRedcost(scip) );
   SCIP_CALL( SCIPincludePropRootredcost(scip) );
   SCIP_CALL( SCIPincludePropSymmetry(scip) );
   SCIP_CALL( SCIPincludePropVbounds(scip) );

   SCIP_CALL( SCIPincludeDispDefault(scip) );
   SCIP_CALL( SCIPincludeTableDefault(scip) );
   SCIP_CALL( SCIPincludeEventHdlrSofttimelimit(scip) );
   SCIP_CALL( SCIPincludeConcurrentScipSolvers(scip) );
   SCIP_CALL( SCIPincludeBendersDefault(scip) );
   SCIP_CALL( SCIPincludeCutselHybrid(scip) );
   SCIP_CALL( SCIPincludeExprhdlrAbs(scip) );
   SCIP_CALL( SCIPincludeExprhdlrCos(scip) );
   SCIP_CALL( SCIPincludeExprhdlrEntropy(scip) );
   SCIP_CALL( SCIPincludeExprhdlrExp(scip) );
   SCIP_CALL( SCIPincludeExprhdlrLog(scip) );
   SCIP_CALL( SCIPincludeExprhdlrPow(scip) );
   SCIP_CALL( SCIPincludeExprhdlrProduct(scip) );
   SCIP_CALL( SCIPincludeExprhdlrSignpower(scip) );
   SCIP_CALL( SCIPincludeExprhdlrSin(scip) );
   SCIP_CALL( SCIPincludeExprhdlrSum(scip) );
   SCIP_CALL( SCIPincludeExprhdlrValue(scip) );
   SCIP_CALL( SCIPincludeExprhdlrVar(scip) );
   SCIP_CALL( SCIPincludeExprhdlrVaridx(scip) );
   SCIP_CALL( SCIPincludeNlhdlrDefault(scip) );
   SCIP_CALL( SCIPincludeNlhdlrConvex(scip) );
   SCIP_CALL( SCIPincludeNlhdlrConcave(scip) );
   SCIP_CALL( SCIPincludeNlhdlrBilinear(scip) );
   SCIP_CALL( SCIPincludeNlhdlrPerspective(scip) );
   SCIP_CALL( SCIPincludeNlhdlrQuadratic(scip) );
   SCIP_CALL( SCIPincludeNlhdlrQuotient(scip) );
   SCIP_CALL( SCIPincludeNlhdlrSoc(scip) );
   SCIP_CALL( SCIPincludeNlpSolverIpopt(scip) );
   SCIP_CALL( SCIPincludeNlpSolverFilterSQP(scip) );
   SCIP_CALL( SCIPincludeNlpSolverWorhp(scip, TRUE) );
   SCIP_CALL( SCIPincludeNlpSolverWorhp(scip, FALSE) );
   SCIP_CALL( SCIPincludeNlpSolverAll(scip) );

   /* Reader: We only care about .srr-files here */
   SCIP_CALL( SCIPincludeReaderSRR(scip) );

   /* Pricer */
   SCIP_CALL( SCIPincludePricerSRR(scip) );

   /* Branching rule */
   SCIP_CALL( SCIPincludeBranchruleSRR(scip) );

   /* Constraint handler for branching decisions */
   SCIP_CALL( SCIPincludeConshdlrSRR(scip) );

   /* for column generation instances, disable restarts */
   SCIP_CALL( SCIPsetIntParam(scip, "presolving/maxrestarts", 0) );

   /* turn off all separation algorithms */
   SCIP_CALL( SCIPsetSeparating(scip, SCIP_PARAMSETTING_OFF, TRUE) );
   SCIP_CALL( SCIPsetIntParam(scip, "separating/maxrounds", 0) );
   SCIP_CALL( SCIPsetIntParam(scip, "separating/maxroundsroot", 0) );

   return SCIP_OKAY;
}
