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

/**@file   reader_srr.c
 * @brief  file reader for SRR
 * @author Jasper van Doornmalen
 */

/*---+----1----+----2----+----3----+----4----+----5----+----6----+----7----+----8----+----9----+----0----+----1----+----2*/
#include "reader_srr.h"

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "probdata_srr.h"

#define READER_NAME             "srrreader"
#define READER_DESC             "file reader for a .srr-file for single-round robin"
#define READER_EXTENSION        "srr"

#define COL_MAX_LINELEN 10000


/*
 * Local methods
 */

/** get next number from string s */
static
SCIP_Real getNextNumber(
   char**                s,                  /**< pointer to the pointer of the current position in the string */
   SCIP_Bool*            success             /**< whether reading succeeded */
   )
{
   SCIP_Real tmp;

   /* skip whitespaces */
   while( isspace(**s) && **s != 0 )
      ++(*s);
   if( **s == 0 )
   {
      *success = FALSE;
      return 0;
   }

   /* read number */
   tmp = atof(*s);
   *success = TRUE;

   /* skip over this number */
   while ( !isspace(**s) && **s != 0 )
      ++(*s);

   return tmp;
}

/** get next number from string s */
static
SCIP_Real getNextNumberInt(
   char**                s,                  /**< pointer to the pointer of the current position in the string */
   SCIP_Bool*            success             /**< whether reading succeeded */
   )
{
   SCIP_Real tmp;

   /* skip whitespaces */
   while( isspace(**s) && **s != 0 )
      ++(*s);
   if( **s == 0 )
   {
      *success = FALSE;
      return 0;
   }

   /* read number */
   tmp = atoi(*s);
   *success = TRUE;

   /* skip over this number */
   while ( !isspace(**s) && **s != 0 )
      ++(*s);

   return tmp;
}

/** read LP in SRR File Format.
 * That means first line is "p edges nbins ncluster".
 * Then a matrix with whitespace-separated entries of size nbins x nbins
*/
static
SCIP_RETCODE readSRR(
   SCIP*                 scip,               /**< SCIP data structure */
   const char*           filename            /**< name of the input file */
   )
{
   SCIP_FILE* fp;                            /* file-reader */
   char buf[COL_MAX_LINELEN];                /* maximal length of line */
   char* char_p;                             /* current char */
   int nteams;                               /* number of teams */
   int nrounds;
   int nelem;
   SCIP_Real* coefs;
   int i;
   int teami;
   int teamj;
   int round;
   SCIP_Real cost;
   SCIP_Bool success;

   assert(scip != NULL);
   assert(filename != NULL);

   if( NULL == (fp = SCIPfopen(filename, "r")) )
   {
      SCIPerrorMessage("cannot open file <%s> for reading\n", filename);
      perror(filename);
      return SCIP_NOFILE;
   }

   /* Get problem name from filename and save it */
   if( SCIPfgets(buf, (int) sizeof(buf), fp) == NULL )
      return SCIP_READERROR;

   char_p = &buf[0];

   /* read out number of nodes and edges, the pointer char_p will be changed */
   nteams = getNextNumberInt(&char_p, &success);
   if( !success )
      return SCIP_READERROR;

   if( nteams <= 0 )
   {
      SCIPerrorMessage("Number of teams must be positive!\n");
      return SCIP_READERROR;
   }

   if( nteams % 1 == 1 )
   {
      SCIPerrorMessage("Number of teams must be even!\n");
      return SCIP_READERROR;
   }

   nrounds = nteams - 1;
   nelem = nteams * nteams * nrounds;

   /* create cmatrix */
   SCIP_CALL( SCIPallocClearBufferArray(scip, &coefs, nelem) );

   /* fill array the cmatrix */
   while( !SCIPfeof(fp) )
   {
      SCIPfgets(buf, (int) sizeof(buf), fp); /*lint !e534*/
      char_p = &buf[0];

      teami = getNextNumberInt(&char_p, &success);
      if( !success )
         continue;  /* Empty line */
      assert( teami >= 0 );
      assert( teami < nteams );

      teamj = getNextNumberInt(&char_p, &success);
      if( !success )
         return SCIP_READERROR;
      assert( teamj >= 0 );
      assert( teamj < nteams );

      round = getNextNumberInt(&char_p, &success);
      if( !success )
         return SCIP_READERROR;
      assert( round >= 0 );
      assert( round < nrounds );

      cost = getNextNumber(&char_p, &success);
      if( !success )
         return SCIP_READERROR;

      i = teami +
         teamj * nteams +
         round * nteams * nteams;
      assert( i >= 0 );
      assert( i < nelem );
      coefs[i] = cost;

      /* Set both coefficients (i, j, r) and (j, i, r), because unordered matches. */
      i = teamj +
         teami * nteams +
         round * nteams * nteams;
      assert( i >= 0 );
      assert( i < nelem );
      coefs[i] = cost;

   }
   SCIPfclose(fp);

#ifndef NDEBUG
   /* Check if coefs is symmetric */
   for (teami = 0; teami < nteams; ++teami)
   {
      for (teamj = teami; teamj < nteams; ++teamj)
      {
         for (round = 0; round < nrounds; ++round)
         {
            assert(
               coefs[teami + teamj * nteams + round * nteams * nteams] ==
               coefs[teamj + teami * nteams + round * nteams * nteams]
            );
         }
      }
   }
#endif

   SCIPinfoMessage(scip, NULL, "Original problem: \n");

   /* create problem data */
   SCIP_CALL( SCIPcreateProbSRR(scip, filename, nteams, nrounds, coefs) );

   SCIPfreeBufferArray(scip, &coefs);

   return SCIP_OKAY;
}

/*
 * Callback methods of reader
 */

/** copy method for reader plugins (called when SCIP copies plugins) */
static
SCIP_DECL_READERCOPY(readerCopySRR)
{
   assert(scip != NULL);
   assert(reader != NULL);
   assert(strcmp( SCIPreaderGetName(reader), READER_NAME) == 0);

   return SCIP_OKAY;
}

/** problem reading method of reader */
static
SCIP_DECL_READERREAD(readerReadSRR)
{
   assert(reader != NULL);
   assert(strcmp( SCIPreaderGetName(reader), READER_NAME) == 0);
   assert(scip != NULL);
   assert(result != NULL);

   SCIP_CALL( readSRR( scip, filename ) );

   *result = SCIP_SUCCESS;

   return SCIP_OKAY;
}

/*
 * srr file reader specific interface methods
 */

/** includes the srr file reader in SCIP */
SCIP_RETCODE SCIPincludeReaderSRR(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_READERDATA* readerdata;
   SCIP_READER* reader;

   /* create srr reader data */
   readerdata = NULL;

   /* include srr reader */
   SCIP_CALL( SCIPincludeReaderBasic( scip, &reader, READER_NAME, READER_DESC, READER_EXTENSION,
      readerdata) );

   SCIP_CALL( SCIPsetReaderCopy( scip, reader, readerCopySRR) );
   SCIP_CALL( SCIPsetReaderRead( scip, reader, readerReadSRR ) );

   SCIP_CALL( SCIPaddCharParam(scip, "srr/model",
      "the model variant", NULL, FALSE, 'y', "xyz", NULL, NULL) );

   return SCIP_OKAY;
}
