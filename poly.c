#define _POSIX_C_SOURCE 202405L

#include <ctype.h>
#include <errno.h>
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <gsl/gsl_poly.h>

#include "poly.g.h"
#include "poly.l.h"

static char *progname;
static int status;

static struct gengetopt_args_info args;

char *
strrep(char *string, char old, char new)
{
	size_t i, len;

	len = strlen(string);
	for (i=0; i<len; i++)
	{
		if (string[i] == old)
			string[i] = new;
	}

	return string;
}

void
print_polynomial(double *coefficients, int order)
{
	int i;
	
	for (i=0; i<(order+1); i++)
	{
		if (!coefficients[order-i])
			continue;

		if (i)
		{
			printf(" %c ", coefficients[order-i]>0 ? '+' : '-');
		}
		else
		{
			if (coefficients[order-i] < 0)
				putchar('-');
		}

		
		if (order-i)
		{
			if (coefficients[order-i] == 1)
			{
				if ((order - i) == 1)
					putchar('x');
				else
					printf("x^%d", order-i);
			}
			else
			{
				if ((order - i) == 1)
					printf("%.3gx", coefficients[order-i]);
				else
					printf("%.3gx^%d", coefficients[order-i], order-i);
			}
		}
		else		
		{
			printf("%g", coefficients[order-i]);
		}		
	}
	printf(": \n");
}

int
set_coefficient(char *num, double *coefficients, size_t i)
{
	int len;
	char *buf;
	
	errno = 0;
	if (!((coefficients[i] = strtod(num, &buf)) || (buf[0] != '\0')))
	{
		if (!args.silent_given)
			fprintf(stderr, "%s: %s: Bad argument\n", progname, num);
		return 1;
	}
	else if (errno)
	{
		if (!args.silent_given)
		{
			len = strlen(progname) + strlen(num) + 3;
			buf = calloc(len, sizeof(char));
			snprintf(buf, len, "%s: %s", progname, num);
			perror(buf);
		}
		return 1;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	int i, j, tok;
	size_t len;
	double *coefficients, *r_coefficients;
	double *roots;
	gsl_poly_complex_workspace *wk;

	progname = basename(argv[0]);

	for (i=0; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			tok = 1;
			
			for (j=1; j<strlen(argv[i]); j++)
			{
				if (!(isdigit(argv[i][j]) || argv[i][j] == '.'))
					tok = 0;
			}

			if (tok)
				argv[i][0] = '_';
		}
	}

	if(ggo(argc, argv, &args))
		return 1;

	if (args.help_given)
	{
		ggo_print_help();
		return 0;
	}
	
	if (args.version_given)
	{
		ggo_print_version();
		return 0;
	}

	if (args.inputs_num)
	{
		coefficients = calloc(args.inputs_num, sizeof(double));

		if (args.inputs_num == 1)
		{
			fprintf(stderr, "%s: Too few coefficients\n", progname);
			return 1;
		}
		
		for (i=0; i<args.inputs_num; i++)
		{
			strrep(args.inputs[i], '_', '-');
	
			set_coefficient(args.inputs[i], coefficients, i);
		}
	
		if (!args.reverse_given)
		{
			r_coefficients = calloc(args.inputs_num, sizeof(double));
			for (i = 0; i<args.inputs_num; i++)
				r_coefficients[(args.inputs_num-1)-i] = coefficients[i];
			memcpy(coefficients, r_coefficients, args.inputs_num*sizeof(double));
		}

		if (args.verbose_given)
			print_polynomial(coefficients, args.inputs_num-1);
		
		wk = gsl_poly_complex_workspace_alloc(args.inputs_num);
		roots = calloc(2*(args.inputs_num-1), sizeof(double));
	
		if (!gsl_poly_complex_solve(coefficients, args.inputs_num, wk, roots))
		{
			for (i=0; i<args.inputs_num-1; i++)
			{
				if (!roots[2*i+1])
					printf("%.6g\t", roots[2*i]);
				else
					printf("%.6g%c%.6gi\t", roots[2*i], roots[2*i+1] >= 0 ? '+' : '-', fabs(roots[2*i+1]));
			}
			putchar('\n');
		}

		free(coefficients);
		free(roots);
		gsl_poly_complex_workspace_free(wk);
	}
	else
	{
		len = 0;
		coefficients = calloc(1, sizeof(double));
		while ((tok = yylex()) != -1)
		{
			if (!tok)
			{
				if (set_coefficient(yytext, coefficients, len++))
				{
					len = 0;
					free(coefficients);
					coefficients = calloc(1, sizeof(double));
					status = 1;
					
					continue;
				}
				
				errno = 0;
				if (((r_coefficients = realloc(coefficients, (len+1)*sizeof(double))) == NULL) || errno)
				{
					perror(progname);
					status = 1;

					len = 0;
					free(coefficients);
					coefficients = calloc(1, sizeof(double));
					continue;
				}
				else
				{
					coefficients = r_coefficients;
				}
			}
			else
			{
				if (len <= 1)
				{
					if (!args.silent_given)
						printf("%s: Too few coefficients\n", progname);
					status = 1;
					continue;
				}

				if (!args.reverse_given)
				{
					r_coefficients = calloc(len, sizeof(double));
					for (i = 0; i<len; i++)
						r_coefficients[(len-1)-i] = coefficients[i];
					memcpy(coefficients, r_coefficients, len*sizeof(double));
				}

				if (args.verbose_given)
					print_polynomial(coefficients, len-1);
				
				wk = gsl_poly_complex_workspace_alloc(len);
				roots = calloc(2*(len-1), sizeof(double));

				if (!gsl_poly_complex_solve(coefficients, len, wk, roots))
				{
					for (i=0; i<len-1; i++)
					{
						if (!roots[2*i+1])
							printf("%.6g\t", roots[2*i]);
						else
							printf("%.6g%c%.6gi\t", roots[2*i], roots[2*i+1] >= 0 ? '+' : '-', fabs(roots[2*i+1]));
					}										
					putchar('\n');
				}

				free(coefficients);
				free(roots);
				gsl_poly_complex_workspace_free(wk);

				len = 0;
				coefficients = calloc(1, sizeof(double));
			}
		}
	}
	
	return args.loose_exit_status_given ? 0 : status;
}
