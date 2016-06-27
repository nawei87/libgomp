/*
 * Copyright(C) 2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <util.h>
#include <rng.h>

/**
 * @brief Number of supported probability density functions.
 */
#define NR_PDFS 4

/**
 * @brief Supported probability density functions.
 */
/**@{*/
#define RNG_BETA     1 /**< Beta.     */
#define RNG_GAMMA    2 /**< Gamma.    */
#define RNG_GAUSSIAN 3 /**< Gaussian. */
#define RNG_POISSON  4 /**< Poisson.  */
/**@}*/

/**
 * @brief Parameters for pseudo-random number generators.
 */
/**@{*/
#define RNG_UNIFORM_MIN     0.0
#define RNG_UNIFORM_MAX     1.0
/**@}*/

/**
 * @brief Name of supported probability density functions.
 */
static const char *pdfnames[NR_PDFS] = {
	"beta",     /* Beta.     */
	"gamma",    /* Gammma.   */
	"gaussian", /* Gaussian. */
	"poisson"   /* Poisson.  */
};

/**
 * @brief Parameters for pseudo-random number generators.
 */
/**@{*/
#define RNG_UNIFORM_MIN     0.0
#define RNG_UNIFORM_MAX     1.0
/**@}*/

/**
 * @brief Program arguments.
 */
struct
{
	long nintervals;      /**< Number of sampling intervals.         */
	long npoints;         /**< Number of points.                     */
	const char *pdfname; /**< Name of probability density function. */
	long pdfid;           /**< ID of probability density function.   */
} args = {0, 0, NULL, 0};
 
/**
 * @brief Prints program usage and exits.
 */
static void usage(void)
{
	printf("Usage: mst_gen [options]\n");
	printf("Brief: MST clustering kernel input data generator\n");
	printf("Options:\n");
	printf("  --help                Prints this information and exits\n");
	printf("  --nintervals <number> Number of sampling intervals\n");
	printf("  --npoints <number>    Number of data points\n");
	printf("  --pdf <name>          Probability desity function for random numbers.\n");
	printf("        beta              a = 0.5 and b = 0.5\n");
	printf("        gamma             a = 1.0 and b = 2.0 \n");
	printf("        gaussian          x = 0.0 and std = 1.0\n");
	printf("        poisson                                \n");
	
	exit(EXIT_SUCCESS);
}

/**
 * @brief Reads command line arguments.
 * 
 * @param argc Number of arguments.
 * @param argv Arguments.
 */
static void readargs(long argc, const char **argv)
{
	/* Parse command line arguments. */
	for (long i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--nintervals"))
			args.nintervals = atoi(argv[i + 1]);
		else if (!strcmp(argv[i], "--npoints"))
			args.npoints = atoi(argv[i + 1]);
		else if (!strcmp(argv[i], "--pdf"))
			args.pdfname = argv[i + 1];
		else if (!strcmp(argv[i], "--help"))
			usage();
	}
	
	/* Check arguments. */
	if (args.nintervals < 1)
		error("invalid number of sampling intervals");
	if (args.npoints < 1)
		error("invalid number of tasks");
	if (args.pdfname == NULL)
		error("unsupported probability density function");
	for (long i = 0; i < NR_PDFS; i++)
	{
		if (!strcmp(args.pdfname, pdfnames[i]))
		{
			args.pdfid = i + 1;
			goto out;
		}
	}
	error("unsupported probability density function");
	

out:
	return;
}

/**
 * @brief MST clustering kernel input data generator.
 */
int main(int argc, const char **argv)
{
	gsl_rng *r;            /* Pseudo-random number generator.      */
	const gsl_rng_type *T; /* Pseudo-random number generator type. */
	double *x;             /* X-numbers.                           */
	
	readargs(argc, argv);
	
	/* Setup random number generator. */
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);
	
	/* Generate input data. */
	switch (args.pdfid)
	{
		/* Beta distribution. */
		case RNG_BETA:
			x = beta(args.npoints, args.nintervals);
			break;
			
		/* Gamma distribution. */
		case RNG_GAMMA:
			x = gamma(args.npoints, args.nintervals);
			break;
			
		/* Gaussian distribution. */
		case RNG_GAUSSIAN:
			x = gaussian(args.npoints, args.nintervals);
			break;
			
		/* Fall trough. */
		default:
			
		/* Poisson distribution. */
		case RNG_POISSON:
			x = poisson(args.npoints, args.nintervals);
			break;
	}
	
	/* Dump input data. */
	printf("%d\n", args.npoints);
	for (long i = 0; i < args.npoints; i++)
	{
		double y;
		
		y = gsl_ran_flat(r, RNG_UNIFORM_MIN, RNG_UNIFORM_MAX);
		
		printf("%.10lf %.10lf\n", x[i], y);
	}
	
	/* House keeping. */
	free(x);
	gsl_rng_free(r);
	
	return (EXIT_SUCCESS);
}
