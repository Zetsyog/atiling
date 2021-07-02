#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
 
/******************************** TILEMIN ********************************/
static inline long int TILEMIN(long int N, long int M) {
  long int TMIN=0,TMP;
  TMP=(long int)N*(long int)M;
  if (TMP > TMIN) TMIN=TMP;
 
 
  return TMIN+1;
} /* end TILEMIN */
 
/******************************** i Ehrhart Polynomials ********************************/
static inline long int i_Ehrhart(long int N, long int M) {
 
  if ((N>0) && ((M>0))) {
    return ((long int)M*(long int)N*((long int)N+(long int)1))/(long int)2;
  }
  fprintf(stderr,"Error i_Ehrhart: no corresponding domain: (N, M) = (%ld, %ld)\n",N, M);
  exit(1);
}  /* end i_Ehrhart */
 
/******************************** i Ranking Polynomials ********************************/
static inline long int i_Ranking(long int i, long int j, long int k,long int N, long int M) {
 
  if ((i<N) && (((0<=j) && (j<=i))) && (((0<=k) && (k<M)))) {
    return ((long int)2*(long int)k+(long int)2*(long int)M*(long int)j+(long int)M*powl((long int)i,(long int)2)+(long int)M*(long int)i+(long int)2)/(long int)2;
  }
  fprintf(stderr,"Error i_Ranking: no corresponding domain: (i, j, k, N, M) = (%ld, %ld, %ld, %ld, %ld)\n",i, j, k,N, M);
  exit(1);
} /* end i_Ranking */
 
/******************************** i_PCMin ********************************/
/******************************** i_PCMin_1 ********************************/
static inline long int i_PCMin_1(long int N, long int M) {
 
  if ((N>=1) && ((M>=1))) {
    return (long int)1;
  }
  return i_Ehrhart(N, M);
} /* end i_PCMin_1 */
 
/******************************** i_PCMax ********************************/
/******************************** i_PCMax_1 ********************************/
static inline long int i_PCMax_1(long int N, long int M) {
 
  if ((N>=1) && ((M>=1))) {
    return ((long int)M*(long int)N*((long int)N+(long int)1))/(long int)2;
  }
  return 0;
} /* end i_PCMax_1 */
 
/******************************** i_trahrhe_i ********************************/
static inline long int i_trahrhe_i(long int pc, long int N, long int M) {
 
  if ( ((M>=1)) && (i_PCMin_1(N, M) <= pc) && (pc <= i_PCMax_1(N, M)) ) {
 
  long int i = floorl(creall((csqrtl((long double)8*(long double)M*(long double)pc+cpowl((long double)M,(long double)2)-(long double)8*(long double)M)-(long double)M)/((long double)2*(long double)M))+0.00000001);
  if ((M>=1) && ((N>=i+1)) && ((i>=0))) {
    return i;
  }
  }
 
  fprintf(stderr,"Error i_trahrhe_i: no corresponding domain: (pc, N, M) = (%ld,%ld, %ld)\n",pc,N, M);
  exit(1);
} /* end i_trahrhe_i */
 
/******************************** i_trahrhe_j ********************************/
static inline long int i_trahrhe_j(long int pc, long int i, long int N, long int M) {
 
  if ( ((M>=1) && ((N>=i+1))) && (i_PCMin_1(N, M) <= pc) && (pc <= i_PCMax_1(N, M)) ) {
 
  long int j = floorl(creall(((long double)2*(long double)pc-(long double)M*cpowl((long double)i,(long double)2)-(long double)M*(long double)i-(long double)2)/((long double)2*(long double)M))+0.00000001);
  if ((M>=1) && ((N>=i+1)) && ((j>=0)) && ((i>=j))) {
    return j;
  }
  }
 
  fprintf(stderr,"Error i_trahrhe_j: no corresponding domain: (pc, i, N, M) = (%ld,%ld, %ld, %ld)\n",pc,i,N, M);
  exit(1);
} /* end i_trahrhe_j */
 
/******************************** i_trahrhe_k ********************************/
static inline long int i_trahrhe_k(long int pc, long int i,long int j, long int N, long int M) {
 
  if ( ((N>=i+1) && ((j>=0)) && ((i>=j))) && (i_PCMin_1(N, M) <= pc) && (pc <= i_PCMax_1(N, M)) ) {
 
  long int k = floorl(creall(((long double)2*(long double)pc-(long double)2*(long double)M*(long double)j-(long double)M*cpowl((long double)i,(long double)2)-(long double)M*(long double)i-(long double)2)/(long double)2)+0.00000001);
  if ((i<N) && (((0<=j) && (j<=i))) && (((0<=k) && (k<M)))) {
    return k;
  }
  }
 
  fprintf(stderr,"Error i_trahrhe_k: no corresponding domain: (pc, i,j, N, M) = (%ld,%ld, %ld, %ld, %ld)\n",pc,i,j,N, M);
  exit(1);
} /* end i_trahrhe_k */
 
/******************************** j Ehrhart Polynomials ********************************/
static inline long int j_Ehrhart(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>0) && ((lbi>=0)) && (((lbi<=ubi) && (ubi<N)))) {
    return ((long int)M*((long int)ubi-(long int)lbi+(long int)1)*((long int)ubi+(long int)lbi+(long int)2))/(long int)2;
  }
  fprintf(stderr,"Error j_Ehrhart: no corresponding domain: (N, M, lbi, ubi) = (%ld, %ld, %ld, %ld)\n",N, M, lbi, ubi);
  exit(1);
}  /* end j_Ehrhart */
 
/******************************** j Ranking Polynomials ********************************/
static inline long int j_Ranking(long int j, long int i, long int k,long int N, long int M, long int lbi, long int ubi) {
 
  if ((lbi>=0) && ((ubi<N)) && ((j>=lbi)) && (((0<=j) && (j<=1+lbi))) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return (long int)M*(long int)j*(long int)ubi-(long int)M*(long int)j*(long int)lbi+(long int)k+(long int)M*(long int)i+(long int)1;
  }
  if ((lbi>=0) && ((ubi<N)) && (((0<=j) && (j<lbi))) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return (long int)M*(long int)j*(long int)ubi-(long int)M*(long int)j*(long int)lbi-(long int)M*(long int)lbi+(long int)k+(long int)M*(long int)j+(long int)M*(long int)i+(long int)1;
  }
  if ((lbi>=0) && ((ubi<N)) && ((j>=2+lbi)) && ((j>=0)) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return ((long int)2*(long int)M*(long int)j*(long int)ubi-(long int)M*powl((long int)lbi,(long int)2)-(long int)M*(long int)lbi+(long int)2*(long int)k-(long int)M*powl((long int)j,(long int)2)+(long int)M*(long int)j+(long int)2*(long int)M*(long int)i+(long int)2)/(long int)2;
  }
  fprintf(stderr,"Error j_Ranking: no corresponding domain: (j, i, k, N, M, lbi, ubi) = (%ld, %ld, %ld, %ld, %ld, %ld, %ld)\n",j, i, k,N, M, lbi, ubi);
  exit(1);
} /* end j_Ranking */
 
/******************************** j_PCMin ********************************/
/******************************** j_PCMin_1 ********************************/
static inline long int j_PCMin_1(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((ubi>=lbi))) {
    return (long int)M*(long int)lbi*(long int)ubi-(long int)M*powl((long int)lbi,(long int)2)+(long int)M*(long int)lbi+(long int)1;
  }
  return j_Ehrhart(N, M, lbi, ubi);
} /* end j_PCMin_1 */
 
/******************************** j_PCMin_2 ********************************/
static inline long int j_PCMin_2(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((lbi>=1)) && ((N>=ubi+1)) && ((ubi>=lbi))) {
    return (long int)1;
  }
  return j_Ehrhart(N, M, lbi, ubi);
} /* end j_PCMin_2 */
 
/******************************** j_PCMin_3 ********************************/
static inline long int j_PCMin_3(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((lbi>=0)) && ((ubi>=lbi+2)) && ((N>=ubi+1))) {
    return ((long int)M*(long int)lbi+(long int)2*(long int)M)*(long int)ubi-(long int)M*powl((long int)lbi,(long int)2)-(long int)M*(long int)lbi+(long int)M+(long int)1;
  }
  return j_Ehrhart(N, M, lbi, ubi);
} /* end j_PCMin_3 */
 
/******************************** j_PCMax ********************************/
/******************************** j_PCMax_1 ********************************/
static inline long int j_PCMax_1(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((N>=lbi+1)) && ((lbi>=0)) && ((ubi==lbi))) {
    return (long int)M*((long int)lbi*(long int)ubi-powl((long int)lbi,(long int)2)+(long int)lbi+(long int)1);
  }
 
  if ((M>=1) && ((lbi>=0)) && ((ubi>=lbi+1)) && ((N>=ubi+1))) {
    return (long int)M*((long int)lbi*(long int)ubi+(long int)2*(long int)ubi-powl((long int)lbi,(long int)2)-(long int)lbi+(long int)1);
  }
  return 0;
} /* end j_PCMax_1 */
 
/******************************** j_PCMax_2 ********************************/
static inline long int j_PCMax_2(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((lbi>=1)) && ((N>=ubi+1)) && ((ubi>=lbi))) {
    return (long int)M*(long int)lbi*((long int)ubi-(long int)lbi+(long int)1);
  }
  return 0;
} /* end j_PCMax_2 */
 
/******************************** j_PCMax_3 ********************************/
static inline long int j_PCMax_3(long int N, long int M, long int lbi, long int ubi) {
 
  if ((M>=1) && ((lbi>=0)) && ((ubi>=lbi+2)) && ((N>=ubi+1))) {
    return ((long int)M*((long int)ubi-(long int)lbi+(long int)1)*((long int)ubi+(long int)lbi+(long int)2))/(long int)2;
  }
  return 0;
} /* end j_PCMax_3 */
 
/******************************** j_trahrhe_j ********************************/
static inline long int j_trahrhe_j(long int pc, long int N, long int M, long int lbi, long int ubi) {
 
  if ( ((M>=1) && ((lbi>=0)) && ((N>=ubi+1))) && (j_PCMin_1(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_1(N, M, lbi, ubi)) ) {
 
  long int j = floorl(creall(((long double)pc-(long double)1)/((long double)M*(long double)ubi-(long double)M*(long double)lbi+(long double)M))+0.00000001);
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi)) && ((ubi>=j)) && ((lbi+1>=j))) {
    return j;
  }
  }
 
  if ( ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi))) && (j_PCMin_2(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_2(N, M, lbi, ubi)) ) {
 
  long int j = floorl(creall(((long double)pc-(long double)1)/((long double)M*(long double)ubi-(long double)M*(long double)lbi+(long double)M))+0.00000001);
  if ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi)) && ((lbi>=j+1)) && ((j>=0))) {
    return j;
  }
  }
 
  if ( ((M>=1) && ((lbi>=0)) && ((N>=ubi+1))) && (j_PCMin_3(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_3(N, M, lbi, ubi)) ) {
 
  long int j = floorl(creall(-(csqrtl((long double)4*cpowl((long double)M,(long double)2)*cpowl((long double)ubi,(long double)2)+(long double)12*cpowl((long double)M,(long double)2)*(long double)ubi-(long double)8*(long double)M*(long double)pc-(long double)4*cpowl((long double)M,(long double)2)*cpowl((long double)lbi,(long double)2)-(long double)4*cpowl((long double)M,(long double)2)*(long double)lbi+(long double)9*cpowl((long double)M,(long double)2)+(long double)8*(long double)M)-(long double)2*(long double)M*(long double)ubi-(long double)3*(long double)M)/((long double)2*(long double)M))+0.00000001);
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi+2)) && ((ubi>=j))) {
    return j;
  }
  }
 
  fprintf(stderr,"Error j_trahrhe_j: no corresponding domain: (pc, N, M, lbi, ubi) = (%ld,%ld, %ld, %ld, %ld)\n",pc,N, M, lbi, ubi);
  exit(1);
} /* end j_trahrhe_j */
 
/******************************** j_trahrhe_i ********************************/
static inline long int j_trahrhe_i(long int pc, long int j, long int N, long int M, long int lbi, long int ubi) {
 
  if ( ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi)) && ((lbi+1>=j))) && (j_PCMin_1(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_1(N, M, lbi, ubi)) ) {
 
  long int i = floorl(creall(-((long double)M*(long double)j*(long double)ubi-(long double)pc-(long double)M*(long double)j*(long double)lbi+(long double)1)/(long double)M)+0.00000001);
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi)) && ((lbi+1>=j)) && ((i>=j)) && ((ubi>=i))) {
    return i;
  }
  }
 
  if ( ((M>=1) && ((N>=ubi+1)) && ((lbi>=j+1)) && ((j>=0))) && (j_PCMin_2(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_2(N, M, lbi, ubi)) ) {
 
  long int i = floorl(creall(-((long double)M*(long double)j*(long double)ubi-(long double)pc-(long double)M*(long double)j*(long double)lbi-(long double)M*(long double)lbi+(long double)M*(long double)j+(long double)1)/(long double)M)+0.00000001);
  if ((M>=1) && ((N>=ubi+1)) && ((lbi>=j+1)) && ((j>=0)) && ((i>=lbi)) && ((ubi>=i))) {
    return i;
  }
  }
 
  if ( ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi+2))) && (j_PCMin_3(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_3(N, M, lbi, ubi)) ) {
 
  long int i = floorl(creall(-((long double)2*(long double)M*(long double)j*(long double)ubi-(long double)2*(long double)pc-(long double)M*cpowl((long double)lbi,(long double)2)-(long double)M*(long double)lbi-(long double)M*cpowl((long double)j,(long double)2)+(long double)M*(long double)j+(long double)2)/((long double)2*(long double)M))+0.00000001);
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((j>=lbi+2)) && ((i>=j)) && ((ubi>=i))) {
    return i;
  }
  }
 
  fprintf(stderr,"Error j_trahrhe_i: no corresponding domain: (pc, j, N, M, lbi, ubi) = (%ld,%ld, %ld, %ld, %ld, %ld)\n",pc,j,N, M, lbi, ubi);
  exit(1);
} /* end j_trahrhe_i */
 
/******************************** j_trahrhe_k ********************************/
static inline long int j_trahrhe_k(long int pc, long int j,long int i, long int N, long int M, long int lbi, long int ubi) {
 
  if ( ((lbi>=0) && ((N>=ubi+1)) && ((j>=lbi)) && ((lbi+1>=j)) && ((i>=j)) && ((ubi>=i))) && (j_PCMin_1(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_1(N, M, lbi, ubi)) ) {
 
  long int k = floorl(creall((-(long double)M*(long double)j*(long double)ubi)+(long double)pc+(long double)M*(long double)j*(long double)lbi-(long double)M*(long double)i-(long double)1)+0.00000001);
  if ((lbi>=0) && ((ubi<N)) && ((j>=lbi)) && (((0<=j) && (j<=1+lbi))) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return k;
  }
  }
 
  if ( ((N>=ubi+1) && ((lbi>=j+1)) && ((j>=0)) && ((i>=lbi)) && ((ubi>=i))) && (j_PCMin_2(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_2(N, M, lbi, ubi)) ) {
 
  long int k = floorl(creall((-(long double)M*(long double)j*(long double)ubi)+(long double)pc+((long double)M*(long double)j+(long double)M)*(long double)lbi-(long double)M*(long double)j-(long double)M*(long double)i-(long double)1)+0.00000001);
  if ((lbi>=0) && ((ubi<N)) && (((0<=j) && (j<lbi))) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return k;
  }
  }
 
  if ( ((lbi>=0) && ((N>=ubi+1)) && ((j>=lbi+2)) && ((i>=j)) && ((ubi>=i))) && (j_PCMin_3(N, M, lbi, ubi) <= pc) && (pc <= j_PCMax_3(N, M, lbi, ubi)) ) {
 
  long int k = floorl(creall(-((long double)2*(long double)M*(long double)j*(long double)ubi-(long double)2*(long double)pc-(long double)M*cpowl((long double)lbi,(long double)2)-(long double)M*(long double)lbi-(long double)M*cpowl((long double)j,(long double)2)+(long double)M*(long double)j+(long double)2*(long double)M*(long double)i+(long double)2)/(long double)2)+0.00000001);
  if ((lbi>=0) && ((ubi<N)) && ((j>=2+lbi)) && ((j>=0)) && ((i>=lbi)) && (((j<=i) && (i<=ubi))) && (((0<=k) && (k<M)))) {
    return k;
  }
  }
 
  fprintf(stderr,"Error j_trahrhe_k: no corresponding domain: (pc, j,i, N, M, lbi, ubi) = (%ld,%ld, %ld, %ld, %ld, %ld, %ld)\n",pc,j,i,N, M, lbi, ubi);
  exit(1);
} /* end j_trahrhe_k */
 
/******************************** k Ehrhart Polynomials ********************************/
static inline long int k_Ehrhart(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>0) && ((lbi>=0)) && (((lbi<=ubi) && (ubi<N))) && ((lbj>=0)) && (((lbj<=ubj) && (ubj<lbi)))) {
    return (long int)M*((long int)ubi-(long int)lbi+(long int)1)*((long int)ubj-(long int)lbj+(long int)1);
  }
  if ((M>0) && ((lbi>=0)) && ((ubi<N)) && ((lbj>=0)) && ((ubj>=lbi)) && (((lbj<=ubj) && (ubj<=ubi)))) {
    return -(long int)M*((long int)ubj-(long int)lbj+(long int)1)*((long int)ubj-(long int)ubi-(long int)1);
  }
  fprintf(stderr,"Error k_Ehrhart: no corresponding domain: (N, M, lbi, ubi, lbj, ubj) = (%ld, %ld, %ld, %ld, %ld, %ld)\n",N, M, lbi, ubi, lbj, ubj);
  exit(1);
}  /* end k_Ehrhart */
 
/******************************** k Ranking Polynomials ********************************/
static inline long int k_Ranking(long int k, long int i, long int j,long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((lbi>=0) && ((ubi<N)) && ((lbj>=0)) && ((ubj<lbi)) && (((0<=k) && (k<M))) && ((i>=lbi)) && (((ubj<=i) && (i<=ubi))) && (((lbj<=j) && (j<=ubj)))) {
    return ((long int)k*(long int)ubi+((-(long int)k)-(long int)1)*(long int)lbi+(long int)k+(long int)i)*(long int)ubj+((long int)k-(long int)k*(long int)lbj)*(long int)ubi+(((long int)k+(long int)1)*(long int)lbi-(long int)k-(long int)i-(long int)1)*(long int)lbj+((-(long int)k)-(long int)1)*(long int)lbi+(long int)k+(long int)j+(long int)i+(long int)1;
  }
  if ((ubj==lbi) && ((lbi>=0)) && ((ubi<N)) && ((lbj>=0)) && (((0<=k) && (k<M))) && (((lbi<=i) && (i<=ubi))) && (((lbj<=j) && (j<=lbi)))) {
    return (long int)ubj*((-(long int)k*(long int)ubj)+(long int)k*(long int)ubi+(long int)k*(long int)lbj-(long int)lbi+(long int)i)+((long int)k-(long int)k*(long int)lbj)*(long int)ubi+((long int)lbi-(long int)k-(long int)i-(long int)1)*(long int)lbj-(long int)lbi+(long int)k+(long int)j+(long int)i+(long int)1;
  }
  if ((lbi>=0) && ((ubi<N)) && ((lbj>=0)) && ((ubj>lbi)) && (((0<=k) && (k<M))) && ((i>=lbi)) && (((ubj<=i) && (i<=ubi))) && (((lbj<=j) && (j<=ubj)))) {
    return (long int)ubj*(((-(long int)k)-(long int)1)*(long int)ubj+(long int)k*(long int)ubi+((long int)k+(long int)1)*(long int)lbj+(long int)i-(long int)1)+((long int)k-(long int)k*(long int)lbj)*(long int)ubi+((-(long int)k)-(long int)i-(long int)1)*(long int)lbj+(long int)k+(long int)j+(long int)i+(long int)1;
  }
  fprintf(stderr,"Error k_Ranking: no corresponding domain: (k, i, j, N, M, lbi, ubi, lbj, ubj) = (%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld)\n",k, i, j,N, M, lbi, ubi, lbj, ubj);
  exit(1);
} /* end k_Ranking */
 
/******************************** k_PCMin ********************************/
/******************************** k_PCMin_1 ********************************/
static inline long int k_PCMin_1(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj))) {
    return (long int)1;
  }
  return k_Ehrhart(N, M, lbi, ubi, lbj, ubj);
} /* end k_PCMin_1 */
 
/******************************** k_PCMin_2 ********************************/
static inline long int k_PCMin_2(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi))) {
    return (long int)1;
  }
  return k_Ehrhart(N, M, lbi, ubi, lbj, ubj);
} /* end k_PCMin_2 */
 
/******************************** k_PCMin_3 ********************************/
static inline long int k_PCMin_3(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((ubi>=ubj))) {
    return (long int)1;
  }
  return k_Ehrhart(N, M, lbi, ubi, lbj, ubj);
} /* end k_PCMin_3 */
 
/******************************** k_PCMax ********************************/
/******************************** k_PCMax_1 ********************************/
static inline long int k_PCMax_1(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj))) {
    return (long int)M*((long int)ubi-(long int)lbi+(long int)1)*((long int)ubj-(long int)lbj+(long int)1);
  }
  return 0;
} /* end k_PCMax_1 */
 
/******************************** k_PCMax_2 ********************************/
static inline long int k_PCMax_2(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((N>=ubi+1)) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi))) {
    return (long int)ubj*(((long int)1-(long int)M)*(long int)ubj+(long int)M*(long int)ubi+((long int)M-(long int)1)*(long int)lbj-(long int)lbi)+((long int)M-(long int)M*(long int)lbj)*(long int)ubi+((long int)lbi-(long int)M)*(long int)lbj+(long int)M;
  }
  return 0;
} /* end k_PCMax_2 */
 
/******************************** k_PCMax_3 ********************************/
static inline long int k_PCMax_3(long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ((M>=1) && ((lbi>=0)) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((ubi>=ubj))) {
    return -(long int)M*((long int)ubj-(long int)lbj+(long int)1)*((long int)ubj-(long int)ubi-(long int)1);
  }
  return 0;
} /* end k_PCMax_3 */
 
/******************************** k_trahrhe_k ********************************/
static inline long int k_trahrhe_k(long int pc, long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ( ((N>=ubi+1) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj))) && (k_PCMin_1(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_1(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int k = floorl(creall(((long double)pc-(long double)1)/(((long double)ubi-(long double)lbi+(long double)1)*((long double)ubj-(long double)lbj+(long double)1)))+0.00000001);
  if ((N>=ubi+1) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj)) && ((M>=k+1)) && ((k>=0))) {
    return k;
  }
  }
 
  if ( ((N>=ubi+1) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi))) && (k_PCMin_2(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_2(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int k = floorl(creall(-((long double)pc-(long double)1)/(((long double)ubj-(long double)lbj+(long double)1)*((long double)ubj-(long double)ubi-(long double)1)))+0.00000001);
  if ((N>=ubi+1) && ((ubi>=lbi)) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi)) && ((M>=k+1)) && ((k>=0))) {
    return k;
  }
  }
 
  if ( ((lbi>=0) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((ubi>=ubj))) && (k_PCMin_3(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_3(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int k = floorl(creall(-((long double)pc-(long double)1)/(((long double)ubj-(long double)lbj+(long double)1)*((long double)ubj-(long double)ubi-(long double)1)))+0.00000001);
  if ((lbi>=0) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((ubi>=ubj)) && ((M>=k+1)) && ((k>=0))) {
    return k;
  }
  }
 
  fprintf(stderr,"Error k_trahrhe_k: no corresponding domain: (pc, N, M, lbi, ubi, lbj, ubj) = (%ld,%ld, %ld, %ld, %ld, %ld, %ld)\n",pc,N, M, lbi, ubi, lbj, ubj);
  exit(1);
} /* end k_trahrhe_k */
 
/******************************** k_trahrhe_i ********************************/
static inline long int k_trahrhe_i(long int pc, long int k, long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ( ((N>=ubi+1) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj)) && ((M>=k+1)) && ((k>=0))) && (k_PCMin_1(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_1(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int i = floorl(creall((((-(long double)k*(long double)ubi)+((long double)k+(long double)1)*(long double)lbi-(long double)k)*(long double)ubj+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+(((-(long double)k)-(long double)1)*(long double)lbi+(long double)k)*(long double)lbj+((long double)k+(long double)1)*(long double)lbi-(long double)k-(long double)1)/((long double)ubj-(long double)lbj+(long double)1))+0.00000001);
  if ((N>=ubi+1) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((ubj>=lbj)) && ((M>=k+1)) && ((k>=0)) && ((i>=lbi)) && ((ubi>=i))) {
    return i;
  }
  }
 
  if ( ((N>=ubi+1) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi)) && ((M>=k+1)) && ((k>=0))) && (k_PCMin_2(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_2(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int i = floorl(creall(((long double)ubj*((long double)k*(long double)ubj-(long double)k*(long double)ubi-(long double)k*(long double)lbj+(long double)lbi)+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+((long double)k-(long double)lbi)*(long double)lbj+(long double)lbi-(long double)k-(long double)1)/((long double)ubj-(long double)lbj+(long double)1))+0.00000001);
  if ((N>=ubi+1) && ((lbj>=0)) && ((lbi>=lbj)) && ((ubj==lbi)) && ((M>=k+1)) && ((k>=0)) && ((i>=lbi)) && ((ubi>=i))) {
    return i;
  }
  }
 
  if ( ((lbi>=0) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((M>=k+1)) && ((k>=0))) && (k_PCMin_3(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_3(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int i = floorl(creall(((long double)ubj*(((long double)k+(long double)1)*(long double)ubj-(long double)k*(long double)ubi+((-(long double)k)-(long double)1)*(long double)lbj+(long double)1)+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+(long double)k*(long double)lbj-(long double)k-(long double)1)/((long double)ubj-(long double)lbj+(long double)1))+0.00000001);
  if ((lbi>=0) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((ubj>=lbj)) && ((M>=k+1)) && ((k>=0)) && ((i>=ubj)) && ((ubi>=i))) {
    return i;
  }
  }
 
  fprintf(stderr,"Error k_trahrhe_i: no corresponding domain: (pc, k, N, M, lbi, ubi, lbj, ubj) = (%ld,%ld, %ld, %ld, %ld, %ld, %ld, %ld)\n",pc,k,N, M, lbi, ubi, lbj, ubj);
  exit(1);
} /* end k_trahrhe_i */
 
/******************************** k_trahrhe_j ********************************/
static inline long int k_trahrhe_j(long int pc, long int k,long int i, long int N, long int M, long int lbi, long int ubi, long int lbj, long int ubj) {
 
  if ( ((N>=ubi+1) && ((lbj>=0)) && ((lbi>=ubj+1)) && ((M>=k+1)) && ((k>=0)) && ((i>=lbi)) && ((ubi>=i))) && (k_PCMin_1(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_1(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int j = floorl(creall(((-(long double)k*(long double)ubi)+((long double)k+(long double)1)*(long double)lbi-(long double)k-(long double)i)*(long double)ubj+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+(((-(long double)k)-(long double)1)*(long double)lbi+(long double)k+(long double)i+(long double)1)*(long double)lbj+((long double)k+(long double)1)*(long double)lbi-(long double)k-(long double)i-(long double)1)+0.00000001);
  if ((lbi>=0) && ((ubi<N)) && ((lbj>=0)) && ((ubj<lbi)) && (((0<=k) && (k<M))) && ((i>=lbi)) && (((ubj<=i) && (i<=ubi))) && (((lbj<=j) && (j<=ubj)))) {
    return j;
  }
  }
 
  if ( ((N>=ubi+1) && ((lbj>=0)) && ((ubj==lbi)) && ((M>=k+1)) && ((k>=0)) && ((i>=lbi)) && ((ubi>=i))) && (k_PCMin_2(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_2(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int j = floorl(creall((long double)ubj*((long double)k*(long double)ubj-(long double)k*(long double)ubi-(long double)k*(long double)lbj+(long double)lbi-(long double)i)+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+((-(long double)lbi)+(long double)k+(long double)i+(long double)1)*(long double)lbj+(long double)lbi-(long double)k-(long double)i-(long double)1)+0.00000001);
  if ((ubj==lbi) && ((lbi>=0)) && ((ubi<N)) && ((lbj>=0)) && (((0<=k) && (k<M))) && (((lbi<=i) && (i<=ubi))) && (((lbj<=j) && (j<=lbi)))) {
    return j;
  }
  }
 
  if ( ((lbi>=0) && ((N>=ubi+1)) && ((lbj>=0)) && ((ubj>=lbi+1)) && ((M>=k+1)) && ((k>=0)) && ((i>=ubj)) && ((ubi>=i))) && (k_PCMin_3(N, M, lbi, ubi, lbj, ubj) <= pc) && (pc <= k_PCMax_3(N, M, lbi, ubi, lbj, ubj)) ) {
 
  long int j = floorl(creall((long double)ubj*(((long double)k+(long double)1)*(long double)ubj-(long double)k*(long double)ubi+((-(long double)k)-(long double)1)*(long double)lbj-(long double)i+(long double)1)+((long double)k*(long double)lbj-(long double)k)*(long double)ubi+(long double)pc+((long double)k+(long double)i+(long double)1)*(long double)lbj-(long double)k-(long double)i-(long double)1)+0.00000001);
  if ((lbi>=0) && ((ubi<N)) && ((lbj>=0)) && ((ubj>lbi)) && (((0<=k) && (k<M))) && ((i>=lbi)) && (((ubj<=i) && (i<=ubi))) && (((lbj<=j) && (j<=ubj)))) {
    return j;
  }
  }
 
  fprintf(stderr,"Error k_trahrhe_j: no corresponding domain: (pc, k,i, N, M, lbi, ubi, lbj, ubj) = (%ld,%ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld)\n",pc,k,i,N, M, lbi, ubi, lbj, ubj);
  exit(1);
} /* end k_trahrhe_j */
 
