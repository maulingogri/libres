/*
   Copyright (C) 2011  Statoil ASA, Norway.

   The file 'rng_config.c' is part of ERT - Ensemble based Reservoir Tool.

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

#include <ert/util/mzran.h>
#include <ert/util/util.h>
#include <ert/util/test_util.h>

#include <ert/config/config_parser.h>
#include <ert/config/config_schema_item.h>

#include <ert/enkf/rng_config.h>
#include <ert/enkf/rng_manager.h>
#include <ert/enkf/config_keys.h>
#include <ert/enkf/enkf_defaults.h>
#include <ert/enkf/model_config.h>


struct rng_config_struct {
  rng_alg_type      type;
  char            * seed_load_file;    /* NULL: Do not store the seed. */
  char            * seed_store_file;   /* NULL: Do not load a seed from file. */
};



void rng_config_set_type( rng_config_type * rng_config , rng_alg_type type) {
  rng_config->type = type;
}

rng_alg_type rng_config_get_type(const rng_config_type * rng_config ) {
  return rng_config->type;
}

const char * rng_config_get_seed_load_file( const rng_config_type * rng_config ) {
  return rng_config->seed_load_file;
}

void rng_config_set_seed_load_file( rng_config_type * rng_config , const char * seed_load_file) {
  rng_config->seed_load_file = util_realloc_string_copy( rng_config->seed_load_file , seed_load_file);
}

const char * rng_config_get_seed_store_file( const rng_config_type * rng_config ) {
  return rng_config->seed_store_file;
}

void rng_config_set_seed_store_file( rng_config_type * rng_config , const char * seed_store_file) {
  rng_config->seed_store_file = util_realloc_string_copy( rng_config->seed_store_file , seed_store_file);
}


static rng_config_type * rng_config_alloc_default(void) {
  rng_config_type * rng_config = util_malloc( sizeof * rng_config);

  rng_config_set_type( rng_config , MZRAN );  /* Only type ... */
  rng_config->seed_store_file = NULL;
  rng_config->seed_load_file = NULL;

  return rng_config;
}

rng_config_type * rng_config_alloc_load_user_config(const char * user_config_file) {
  config_parser_type * config_parser = config_alloc();
  config_content_type * config_content = NULL;
  if(user_config_file)
    config_content = model_config_alloc_content(user_config_file, config_parser);

  rng_config_type * rng_config = rng_config_alloc(config_content);

  config_content_free(config_content);
  config_free(config_parser);

  return rng_config;
}

rng_config_type * rng_config_alloc(const config_content_type * config_content) {
  rng_config_type * rng_config = rng_config_alloc_default();

  if(config_content)
    rng_config_init(rng_config, config_content);

  return rng_config;
}

void rng_config_free( rng_config_type * rng) {
  util_safe_free( rng->seed_load_file );
  util_safe_free( rng->seed_store_file );
  free( rng );
}


rng_manager_type * rng_config_alloc_rng_manager( const rng_config_type * rng_config ) {
  const char * seed_store = rng_config_get_seed_store_file( rng_config );
  const char * seed_load  = rng_config_get_seed_load_file( rng_config );
  rng_manager_type * rng_manager;

  if (seed_load && util_file_exists( seed_load ))
    rng_manager = rng_manager_alloc( seed_load );
  else
    rng_manager = rng_manager_alloc_random( );

  if (seed_store)
    rng_manager_save_state( rng_manager , seed_store );

  return rng_manager;
}


/*****************************************************************/

void rng_config_add_config_items( config_parser_type * config ) {
  config_schema_item_type * item;

  item= config_add_schema_item( config , STORE_SEED_KEY , false);
  config_schema_item_set_argc_minmax(item , 1 , 1 );
  config_schema_item_iset_type( item , 0 , CONFIG_PATH );

  item = config_add_schema_item( config , LOAD_SEED_KEY , false );
  config_schema_item_set_argc_minmax(item , 1 , 1 );
  config_schema_item_iset_type( item , 0 , CONFIG_PATH );
}


void rng_config_init(rng_config_type * rng_config, const config_content_type * config_content) {
  if(config_content_has_item(config_content, STORE_SEED_KEY))
    rng_config_set_seed_store_file(rng_config, config_content_iget(config_content, STORE_SEED_KEY, 0, 0));

  if(config_content_has_item(config_content, LOAD_SEED_KEY))
    rng_config_set_seed_load_file(rng_config, config_content_iget(config_content, LOAD_SEED_KEY,0 ,0));
}


void rng_config_fprintf_config( rng_config_type * rng_config , FILE * stream ) {
  if (rng_config->seed_load_file != NULL) {
    fprintf( stream , CONFIG_KEY_FORMAT      , LOAD_SEED_KEY );
    fprintf( stream , CONFIG_ENDVALUE_FORMAT , rng_config->seed_load_file);
  }

  if (rng_config->seed_store_file != NULL) {
    fprintf( stream , CONFIG_KEY_FORMAT      , STORE_SEED_KEY );
    fprintf( stream , CONFIG_ENDVALUE_FORMAT , rng_config->seed_store_file);
  }
}
