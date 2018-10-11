
# options
#########################
lengths <- seq.int(from = {start}, to = {end}, by = {tick});

global_path <- "error/results/";

output_mse <- paste(global_path, "values/mse/MSE_", sep="");
output_rmse <- paste(global_path, "values/rmse/RMSE_", sep="");
output_cor <- paste(global_path, "../misc/stat_err.dat", sep="");
input_missingmat <- paste(global_path, "recovered_matrices/recoveredMat", sep="");

list_algos <- c({allAlgos});

#########################

algos_str = paste(list_algos, collapse="\t");

TITLEBAR=paste("=====================================================",
	paste(" #  \t||  ref\t\t", algos_str, sep=""),
	"=====================================================\n", sep="\n");

SEPARATE="=====================================================\n";

msqe <- function() {
	dftest <- read.table(paste(input_missingmat, lengths[1], ".txt", sep=""), header=FALSE);

	for(i in 2:length(dftest)) {
		fileName = paste(output_mse, list_algos[i-1], ".dat", sep="");
		write(paste("#", list_algos[i-1]), fileName); #rewrite
	}

	for(len in lengths) {
		df <- read.table(paste(input_missingmat, len, ".txt", sep=""), header=FALSE);
		dfmx <- as.matrix(df);
		ref = dfmx[,1];
	
		for(i in 2:length(df)) {
			fileName = paste(output_mse, list_algos[i-1], ".dat", sep="");
			comp <- dfmx[,i];
			comp <- comp - ref;
			msqe_val <- mean(comp^2);
			lin <- paste(len, " ", msqe_val, sep="");
			write(lin, fileName, append=TRUE);
		}
	}
}

rmsqe <- function() {
	dftest <- read.table(paste(input_missingmat, lengths[1], ".txt", sep=""), header=FALSE);

	for(i in 2:length(dftest)) {
		fileName = paste(output_rmse, list_algos[i-1], ".dat", sep="");
		write(paste("#", list_algos[i-1]), fileName); #rewrite
	}

	for(len in lengths) {
		df <- read.table(paste(input_missingmat, len, ".txt", sep=""), header=FALSE);
		dfmx <- as.matrix(df);
		ref = dfmx[,1];
	
		for(i in 2:length(df)) {
			fileName = paste(output_rmse, list_algos[i-1], ".dat", sep="");
			comp <- dfmx[,i];
			comp <- comp - ref;
			msqe_val <- sqrt(mean(comp^2));
			lin <- paste(len, " ", msqe_val, sep="");
			write(lin, fileName, append=TRUE);
		}
	}
}

corr <- function() {
	write("(pearson)", output_cor); #rewrite
	cat(TITLEBAR, file=output_cor, append=TRUE);

	for(i in lengths) {
		df <- read.table(paste(input_missingmat, i, ".txt", sep=""), header=FALSE);
		cat(i, file=output_cor, append=TRUE);
		cat(" \t|| \t", file=output_cor, append=TRUE);
		mat <- cor(df, method="pearson");
		mat = round(mat * 100);
		cat(mat[,1], file=output_cor, sep="\t\t", append=TRUE);
		cat("\n", file=output_cor, append=TRUE);
	}

	cat(SEPARATE, file=output_cor, append=TRUE);
	cat("\n\n(spearman)\n", file=output_cor, append=TRUE);
	cat(TITLEBAR, file=output_cor, append=TRUE);

	for(i in lengths) {
		df <- read.table(paste(input_missingmat, i, ".txt", sep=""), header=FALSE);
		cat(i, file=output_cor, append=TRUE);
		cat(" \t|| \t", file=output_cor, append=TRUE);
		mat <- cor(df, method="spearman");
		mat = round(mat * 100);
		cat(mat[,1], file=output_cor, sep="\t\t", append=TRUE);
		cat("\n", file=output_cor, append=TRUE);
	}

	cat(SEPARATE, file=output_cor, append=TRUE);
	cat("\n\n(kendall)\n", file=output_cor, append=TRUE);
	cat(TITLEBAR, file=output_cor, append=TRUE);

	for(i in lengths) {
		df <- read.table(paste(input_missingmat, i, ".txt", sep=""), header=FALSE);
		cat(i, file=output_cor, append=TRUE);
		cat(" \t|| \t", file=output_cor, append=TRUE);
		mat <- cor(df, method="kendall");
		mat = round(mat * 100);
		cat(mat[,1], file=output_cor, sep="\t\t", append=TRUE);
		cat("\n", file=output_cor, append=TRUE);
	}

	cat(SEPARATE, file=output_cor, append=TRUE);
	cat("\n", file=output_cor, append=TRUE);
}

corr();
msqe();
rmsqe();
