/*
   Copyright (C) 2017  Statoil ASA, Norway.

   The file 'rng_manager.c' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/

#include <stdlib.h>

#include <ert/util/rng.h>
#include <ert/util/vector.h>
#include <ert/enkf/rng_manager.h>

#define RNG_MANAGER_TYPE_ID 77250451


struct rng_manager_struct {
  UTIL_TYPE_ID_DECLARATION;
  rng_alg_type  rng_alg;
  rng_type    * internal_seed_rng;   /* This is used to seed the RNG's which are managed. */
  rng_type    * external_seed_rng;   /* This is used to seed the RNG's which are managed by external scope. */
  vector_type * rng_list;
};


UTIL_IS_INSTANCE_FUNCTION( rng_manager, RNG_MANAGER_TYPE_ID )

static rng_manager_type * rng_manager_alloc__( const char * seed_file, rng_init_mode init_mode) {
  rng_manager_type * rng_manager = util_malloc( sizeof * rng_manager );
  UTIL_TYPE_ID_INIT( rng_manager, RNG_MANAGER_TYPE_ID );
  rng_manager->rng_list = vector_alloc_new( );
  rng_manager->rng_alg = MZRAN;
  rng_manager->internal_seed_rng = rng_alloc( rng_manager->rng_alg, init_mode );
  rng_manager->external_seed_rng = rng_alloc( rng_manager->rng_alg, init_mode );

  if (seed_file) {
    rng_load_state( rng_manager->internal_seed_rng , seed_file );
    rng_load_state( rng_manager->external_seed_rng , seed_file );
  }
  rng_rng_init( rng_manager->external_seed_rng, rng_manager->external_seed_rng );

  return rng_manager;
}


rng_manager_type * rng_manager_alloc( const char * seed_file ) {
  if (!util_file_exists( seed_file ))
    return NULL;

  return rng_manager_alloc__( seed_file, INIT_DEFAULT );
}

rng_manager_type * rng_manager_alloc_default( ) {
  return rng_manager_alloc__( NULL, INIT_DEFAULT );
}

rng_manager_type * rng_manager_alloc_random( ) {
  return rng_manager_alloc__( NULL, INIT_DEV_URANDOM);
}



void rng_manager_free( rng_manager_type * rng_manager ) {
  vector_free( rng_manager->rng_list );
  rng_free( rng_manager->internal_seed_rng );
  rng_free( rng_manager->external_seed_rng );
  free( rng_manager );
}


static void rng_manager_grow(rng_manager_type * rng_manager, int min_size) {
  int new_size = util_int_max( min_size, 2*vector_get_size( rng_manager->rng_list ));
  for (int i = vector_get_size( rng_manager->rng_list ); i < new_size; i++) {
    rng_type * rng = rng_alloc( rng_manager->rng_alg, INIT_DEFAULT );
    rng_rng_init( rng, rng_manager->internal_seed_rng );
    vector_append_owned_ref( rng_manager->rng_list , rng, rng_free__ );
  }
}


rng_type * rng_manager_alloc_rng(rng_manager_type * rng_manager) {
  rng_type * rng = rng_alloc( rng_manager->rng_alg, INIT_DEFAULT );
  rng_rng_init( rng, rng_manager->external_seed_rng );
  return rng;
}


rng_type * rng_manager_iget(rng_manager_type * rng_manager, int index) {
  if (index >= vector_get_size( rng_manager->rng_list ))
    rng_manager_grow( rng_manager, index + 1);

  return vector_iget( rng_manager->rng_list , index );
}



void rng_manager_save_state(const rng_manager_type * rng_manager, const char * seed_file) {
  rng_save_state( rng_manager->internal_seed_rng, seed_file );
}
