#include "niml.h"

/********************************************
  Timing test for NIML element creation:
    - load 1 row at a time
    - load all rows at a time
    - load by columns
*********************************************/

typedef struct {      /* a location in space */
  int id ;
  float x,y,z ;
} AGNI_nod ;

NI_element * AGNI_nod_to_NIML_col ( int num_nod , AGNI_nod *nod ) ;
NI_element * AGNI_nod_to_NIML_row ( int num_nod , AGNI_nod *nod ) ;
NI_element * AGNI_nod_to_NIML_rows( int num_nod , AGNI_nod *nod ) ;

int main( int argc , char *argv[] )
{
   int ii , nn , ct , nb ;
   AGNI_nod *nod ;
   NI_element *nel ;
   NI_stream ns ;

   if( argc < 2 ){ printf("Usage: nisurf n\n"); exit(0); }

   nn = strtol(argv[1],NULL,10) ;
   if( nn < 1 ) exit(1) ;

   /* make up some fake nodes */

   nod = NI_malloc( sizeof(AGNI_nod)*nn ) ;
   for( ii=0 ; ii < nn ; ii++ ){
      nod[ii].id = ii     ;
      nod[ii].x  = 1.1*ii ;
      nod[ii].y  = 2.2*ii ;
      nod[ii].z  = 3.3*ii ;
   }

   /* put nodes into a NIML data element */

   ct = NI_clock_time() ;  /* start timer */
   nel = AGNI_nod_to_NIML_row( nn , nod ) ;
   fprintf(stderr,"Clock time for %d nodes by row = %d ms\n",nn,NI_clock_time()-ct) ;
   if( nn < 10 ){
     ns = NI_stream_open( "fd:1" , "w" ) ;
     if( ns == NULL ){ fprintf(stderr,"Can't open fd:1\n"); exit(1); }
     nb = NI_write_element( ns , nel , NI_TEXT_MODE ) ;
     fprintf(stderr,"num bytes=%d\n\n",nb) ;
   }
   NI_free_element(nel) ;

   /* put nodes into a NIML data element */

   ct = NI_clock_time() ;  /* start timer */
   nel = AGNI_nod_to_NIML_rows( nn , nod ) ;
   fprintf(stderr,"Clock time for %d nodes by rows = %d ms\n",nn,NI_clock_time()-ct) ;
   if( nn < 10 ){
     nb = NI_write_element( ns , nel , NI_TEXT_MODE ) ;
     fprintf(stderr,"num bytes=%d\n\n",nb) ;
   }
   NI_free_element(nel) ;

   /* put nodes into a NIML data element */

   ct = NI_clock_time() ;  /* start timer */
   nel = AGNI_nod_to_NIML_col( nn , nod ) ;
   fprintf(stderr,"Clock time for %d nodes by col = %d ms\n",nn,NI_clock_time()-ct) ;
   if( nn < 10 ){
     nb = NI_write_element( ns , nel , NI_TEXT_MODE ) ;
     fprintf(stderr,"num bytes=%d\n\n",nb) ;
   }
   NI_free_element(nel) ;

   fprintf(stderr,"\n*** That's all folks ***\n") ;
   exit(0) ;
}

/*------------------------------------------------------------------*/
/*! Make a NIML data element from an array of AGNI_nod structs.
    Return value is NULL if you input stupid values.
--------------------------------------------------------------------*/

NI_element * AGNI_nod_to_NIML_row( int num_nod , AGNI_nod *nod )
{
   NI_element *nel ;
   int ii ;

   /* check inputs for sanity */

   if( num_nod < 1 || nod == NULL ) return NULL ;

   /* make a new data element, to be filled by rows */

   nel = NI_new_data_element( "surfixyz" , -1 ) ;

   /* define the mapping from struct components to element columns */

   NI_define_rowmap_VA( nel ,
                          NI_INT   , offsetof(AGNI_nod,id) ,
                          NI_FLOAT , offsetof(AGNI_nod,x)  ,
                          NI_FLOAT , offsetof(AGNI_nod,y)  ,
                          NI_FLOAT , offsetof(AGNI_nod,z)  ,
                        -1 ) ;

   /* load the structs */

   for( ii=0 ; ii < num_nod ; ii++ )
      NI_add_row( nel , nod+ii ) ;

   return nel ;
}

/*------------------------------------------------------------------*/
/*! Make a NIML data element from an array of AGNI_nod structs.
    Return value is NULL if you input stupid values.
--------------------------------------------------------------------*/

NI_element * AGNI_nod_to_NIML_col( int num_nod , AGNI_nod *nod )
{
   NI_element *nel ;
   int ii ;
   int *ic ; float *xc,*yc,*zc ;

   /* check inputs for sanity */

   if( num_nod < 1 || nod == NULL ) return NULL ;

   /* make a new data element, to be filled by columns */

   nel = NI_new_data_element( "surfixyz" , num_nod ) ;

   /* make the columns to be put in the element */

   ic = (int *)   malloc( sizeof(int)   * num_nod ) ;
   xc = (float *) malloc( sizeof(float) * num_nod ) ;
   yc = (float *) malloc( sizeof(float) * num_nod ) ;
   zc = (float *) malloc( sizeof(float) * num_nod ) ;

   /* load the columns */

   for( ii=0 ; ii < num_nod ; ii++ ){
      ic[ii] = nod[ii].id ;
      xc[ii] = nod[ii].x ;
      yc[ii] = nod[ii].y ;
      zc[ii] = nod[ii].z ;
   }

   NI_add_column( nel , NI_INT   , ic ) ; free(ic) ;
   NI_add_column( nel , NI_FLOAT , xc ) ; free(xc) ;
   NI_add_column( nel , NI_FLOAT , yc ) ; free(yc) ;
   NI_add_column( nel , NI_FLOAT , zc ) ; free(zc) ;

   return nel ;
}

/*------------------------------------------------------------------*/
/*! Make a NIML data element from an array of AGNI_nod structs.
    Return value is NULL if you input stupid values.
--------------------------------------------------------------------*/

NI_element * AGNI_nod_to_NIML_rows( int num_nod , AGNI_nod *nod )
{
   NI_element *nel ;
   int ii ;
   int *ic ; float *xc,*yc,*zc ;

   /* check inputs for sanity */

   if( num_nod < 1 || nod == NULL ) return NULL ;

   nel = NI_new_data_element( "surfixyz" , -1 ) ;

   NI_define_rowmap_VA( nel ,
                          NI_INT   , offsetof(AGNI_nod,id) ,
                          NI_FLOAT , offsetof(AGNI_nod,x)  ,
                          NI_FLOAT , offsetof(AGNI_nod,y)  ,
                          NI_FLOAT , offsetof(AGNI_nod,z)  , -1 ) ;

   NI_add_many_rows( nel , num_nod , sizeof(AGNI_nod) , nod ) ;

   return nel ;
}

/*------------------------------------------------------------------*/
/*! Unload a <surfixyz> NI_element into an array of newly
    malloc()-ed AGNI_nod.
    Return value is number of nodes (will be 0 if an error occurs).
    *nod will point to the output array if the number of nodes is
    positive.
--------------------------------------------------------------------*/

int NIML_to_AGNI_nod( NI_element *nel , AGNI_nod **nod )
{
   int   num_nod , ii ;
   AGNI_nod *mynod ;

   /* check element for correctness */

   if( nel             == 0        ||   /* no data element?          */
       nel->vec_len    <  1        ||   /* empty element?             */
       nel->vec_filled <  1        ||   /* no data was filled in?      */
       nel->vec_num    <  4        ||   /* less than 4 columns?         */
       nel->vec_typ[0] != NI_INT   ||   /* must be int,float,float,float */
       nel->vec_typ[1] != NI_FLOAT ||
       nel->vec_typ[2] != NI_FLOAT ||
       nel->vec_typ[3] != NI_FLOAT   ) return 0 ;

   /* number of nodes is number of completely filled rows */

   num_nod = nel->vec_filled ;

   /* make space for the structs */

   mynod = (AGNI_nod *) malloc( sizeof(AGNI_nod) * num_nod ) ;

   /* define the mapping from struct components to element columns */

   NI_define_rowmap_VA( nel ,
                          NI_INT   , offsetof(AGNI_nod,id) ,
                          NI_FLOAT , offsetof(AGNI_nod,x)  ,
                          NI_FLOAT , offsetof(AGNI_nod,y)  ,
                          NI_FLOAT , offsetof(AGNI_nod,z)  ,
                        -1 ) ;

   /* load data from NIML element to the structs */

   for( ii=0 ; ii < num_nod ; ii++ )
      NI_get_row( nel , ii , mynod+ii ) ;

   /* we is done */

   *nod = mynod ; return num_nod ;
}
