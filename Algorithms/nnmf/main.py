import numpy as np
from nnmf import nnmf_impute


def recover(filename, output, runtime, rtstream):
    instream = open(filename, "r");
    lines = instream.readlines();

    nrows = len(lines);

    mat = [[] for _ in xrange(nrows)];

    for i in xrange(nrows):
        arr = lines[i].replace('\n', '').split(' ');
        mat[i] = np.array(map(float, arr));
    # end for

    instream.close();

    ncols = len(mat[0]);

    start = -1;
    end = -1;
    missingmask = [[False for _ in xrange(ncols)] for _ in xrange(nrows)];

    for i in xrange(nrows):
        for j in xrange(ncols):
            if np.isnan(mat[i][j]):
                missingmask[i][j] = True;
    # end for

    print("entering nnmf: n=" + str(nrows) + "; m=" + str(ncols));

    (result, time) = nnmf_impute(mat, nrows, ncols, 3, np.array(missingmask), True, rtstream);

    outstream = open(output, "w");  # rewrites
    if runtime:
        outstream.write(str(time * 1000));  # to ms
    else:
        for i in xrange(nrows):
            for j in xrange(ncols - 1):
                outstream.write(str(result[i][j]) + " ");
            # end for
            outstream.write(str(result[i][ncols-1]) + "\n");
        # end for
        outstream.close();
    # end if
# end function


def main():
    recover("_data/in/climate_m12.txt", "_data/out/nnmf12.txt", True, False);


if __name__ == '__main__':
    main();


