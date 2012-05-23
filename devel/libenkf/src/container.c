/*
   Copyright (C) 2012  Statoil ASA, Norway. 
    
   The file 'container.c' is part of ERT - Ensemble based Reservoir Tool. 
    
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

#include <util.h>
#include <vector.h>


#include <container_config.h>
#include <container.h>
#include <enkf_types.h>
#include <enkf_node.h>

typedef struct container_struct {
  int                          __type_id;       
  container_config_type      * config; 
  vector_type                * nodes;
};



container_type * container_alloc( container_config_type * config ) {
  container_type * container = util_malloc( sizeof * container , __func__ );
  UTIL_TYPE_ID_INIT( container , CONTAINER );
  container->config = config;
  container->nodes  = vector_alloc_new();
  return container;
}


void container_free( container_type * container ) {
  vector_free( container->nodes );
  free( container );
}

/******************************************************************/
/* Anonumously generated functions used by the enkf_node object   */
/******************************************************************/

UTIL_SAFE_CAST_FUNCTION(container , CONTAINER)
UTIL_SAFE_CAST_FUNCTION_CONST(container , CONTAINER)
VOID_ALLOC(container)
VOID_FREE(container)






  
