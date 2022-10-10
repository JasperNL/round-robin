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

#include <iostream>
#include <lemon/smart_graph.h>
#include <lemon/matching.h>

#include "lemon_wrapper.h"

#include <vector>

#ifdef __cplusplus
  extern "C" {
#endif

void SCIPmatchingSolveSRR(
   int nteams,
   double* weights,
   unsigned int* edgeexists,
   double* solutionvalue,
   int* solution
)
{
   int i;
   int j;
   int k;
   int nodeid;

   std::vector<lemon::SmartGraphBase::Node> nodes;

   // assert( weights != NULL );
   // assert( edgeexists != NULL );

   lemon::SmartGraph sgr;
   lemon::SmartGraph::EdgeMap<double> scost(sgr);

   for (i = 0; i < nteams; ++i)
   {
      lemon::SmartGraphBase::Node mynode;
      mynode = sgr.addNode();
      nodes.push_back(mynode);
   }

   k = -1;
   for (i = 0; i < nteams; ++i)
   {
      for (j = i + 1; j < nteams; ++j)
      {
         ++k;
         /* edgeexists and weights arrays follow the natural unordered edge sorting,
          * that (i,j) < (i',j') if i < i', or i = i' and j < j'.
          */
         if( !edgeexists[k] )
            continue;

         lemon::SmartGraph::Edge e =
            sgr.addEdge(sgr.nodeFromId(i), sgr.nodeFromId(j));

         scost[e] = weights[k];
      }
   }

   // Compute min. cost perfect matching
   lemon::MaxWeightedPerfectMatching<lemon::SmartGraph, lemon::SmartGraph::EdgeMap<double>>
      mwpm(sgr, scost);
   mwpm.run();

   *solutionvalue = (double) mwpm.matchingWeight();

   //assert( SCIPisEQ(scip, mwpm.matchingWeight(), mwpm.dualValue()) );

   /* retrieve the matching */
   for (k = 0; k < nteams; ++k)
   {
      nodeid = sgr.id(mwpm.mate(nodes[k]));
      /* this must be possible in a more efficient way ... */
      for (i = 0; i < nteams; ++i)
      {
         if( sgr.id(nodes[i]) == nodeid )
            break;
      }
      // assert( i < nteams );
      solution[k] = i;
   }
}

#ifdef __cplusplus
  }
#endif
