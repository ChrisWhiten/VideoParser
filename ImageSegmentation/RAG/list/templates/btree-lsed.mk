#!/bin/sh
echo "s/btree_t/btree$1_t/g"
echo "s/btree_nil/btree$1_nil/g"
echo "s/btree_Nil/btree$1_Nil/g"
echo "s/btree_free/btree$1_free/g"
echo "s/btree_Free/btree$1_Free/g"
echo "s/btree_setNil/btree$1_setNil/g"
echo "s/btree_realloc/btree$1_realloc/g"
echo "s/btree_cpy/btree$1_cpy/g"
echo "s/btree_Cpy/btree$1_Cpy/g"
echo "s/btree_newParent/btree$1_newParent/g"
echo "s/btree_newNode/btree$1_newNode/g"
echo "s/btree_addParent/btree$1_addParent/g"
echo "s/btree_addNode/btree$1_addNode/g"
