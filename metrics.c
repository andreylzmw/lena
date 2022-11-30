#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>
#include <unistd.h>


double calc_MSE(int m, int n, int **I, int **K) {
	double res = 0;
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			res += pow(abs(I[i][j] - K[i][j]), 2);
	res /= m*n;
	return res;
}

double MSE(char *I_path, char *K_path) {

	printf("I_path = %s\n", I_path);
	printf("K_path = %s\n", K_path);

	FILE *I_file = fopen(I_path, "r");
	FILE *K_file = fopen(K_path, "r");

	int m = 1;
	int n = 1;

	int I_path_size = 0;
	int K_path_size = 0;

	char I_in;
	while (1) {
		fread(&I_in, 1, 1, I_file);
		if (I_in == '\n') break;
		I_path_size++;
	}

	char K_in;
	while (1) {
		fread(&K_in, 1, 1, K_file);
		if (K_in == '\n') break;
		K_path_size++;
	}

	fseek(I_file, 0, SEEK_SET);
	fseek(K_file, 0, SEEK_SET);

	printf("I_path_size = %d\n", I_path_size);
	printf("K_path_size = %d\n", K_path_size);

	char I_tmp_path[I_path_size];
	char K_tmp_path[K_path_size];

	fscanf(I_file, "%s\n", I_tmp_path);
	fscanf(K_file, "%s\n", K_tmp_path);

	fscanf(I_file, "%d %d", &m, &n);
	fscanf(K_file, "%d %d", &m, &n);

	printf("I_tmp_path = %s\n", I_tmp_path);
	printf("K_tmp_path = %s\n", K_tmp_path);

	printf("m = %d\n", m);
	printf("n = %d\n", n);

	int **I_R = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) I_R[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			I_R[i][j] = 0;

	int **I_G = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) I_G[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			I_G[i][j] = 0;

	int **I_B = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) I_B[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			I_B[i][j] = 0;


	int **K_R = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) K_R[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			K_R[i][j] = 0;

	int **K_G = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) K_G[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			K_G[i][j] = 0;

	int **K_B = malloc(sizeof(int*) * m);
	for (int i = 0; i < m; i++) K_B[i] = malloc(sizeof(int) * n);
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			K_B[i][j] = 0;

   

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			fscanf(I_file, "%d %d %d", &I_R[i][j], &I_G[i][j], &I_B[i][j]);

	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			fscanf(K_file, "%d %d %d", &K_R[i][j], &K_G[i][j], &K_B[i][j]);

	double R_MSE = calc_MSE(m, n, I_R, K_R);
	double G_MSE = calc_MSE(m, n, I_G, K_G);
	double B_MSE = calc_MSE(m, n, I_B, K_B);

	printf("R_MSE = %f\n", R_MSE);
	printf("G_MSE = %f\n", G_MSE);
	printf("B_MSE = %f\n", B_MSE);

	double res = (R_MSE + G_MSE + B_MSE) / 3;
	printf("MSE = %f\n", res);

	fclose(I_file);
	fclose(K_file);

	for (int i = 0; i < m; i++) free(I_R[i]);
	free(I_R);

	for (int i = 0; i < m; i++) free(I_G[i]);
	free(I_G);

	for (int i = 0; i < m; i++) free(I_B[i]);
	free(I_B);


	for (int i = 0; i < m; i++) free(K_R[i]);
	free(K_R);

	for (int i = 0; i < m; i++) free(K_G[i]);
	free(K_G);

	for (int i = 0; i < m; i++) free(K_B[i]);
	free(K_B);

	return res;
}

int main() {
	double res = 0;
	int n = 0;
	FILE *database = fopen("database.txt", "r");
	char c;
	for (c = getc(database); c != EOF; c = getc(database)) if (c == '\n') n++;
	printf("n = %d\n", n);
	fseek(database, 0, SEEK_SET);
	for (int i = 0; i < n/2; i++) {
		int I_path_size = 0;
		int K_path_size = 0;

		for (c = getc(database); c != EOF; c = getc(database))
			if (c == ' ') break;
			else I_path_size++;

		
		for (c = getc(database); c != EOF; c = getc(database))
			if (c == '\n') break;
			else K_path_size++;

		char I_path[I_path_size];
		char K_path[K_path_size];

		fscanf(database, "%s %s", I_path, K_path);

		res += MSE(I_path, K_path);
	}
	res /= n/2;
	fclose(database);
	printf("AVERAGE MSE = %f\n", res);
	return 0;
}