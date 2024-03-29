/**
 * @file test-hashtable.c
 * @brief test code for hashtables
 *
 * @author Valérian Rousset & Jean-Cédric Chappelier
 * @date 02 Oct 2017
 */

#include <stdio.h> // for puts(). to be removed when no longer needed.

#include <check.h>

#include "tests.h"

#include "hashtable.c"

#include "node_list.h"

#include "config.h"

#include <arpa/inet.h>

#include "args.h"
#include "node.h"
#include "ring.h"

void print_sha(unsigned char* input)
{
    if (input != NULL) {
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
            printf("%02x", input[i]);
        }
    }
}

START_TEST(add_get_hashtable)
{
//	printf("\nSTARTING TESTS FOR HTABLE\n\n");
    Htable_t table = construct_Htable(HTABLE_SIZE);

    // Defining tests ressource
    const pps_key_t key1 = "antoine";
    const pps_value_t value1 = "crettenand";
    const pps_key_t key2 = "julien";
    const pps_value_t value2 = "malka";
    const pps_key_t key3 = "forgot_in_table";

    const pps_value_t value3 = "theboss";

    add_Htable_value(table, key1, value1);
    add_Htable_value(table, key2, value2);

    const pps_value_t value_read1 = get_Htable_value(table, key1);

    ck_assert_str_eq(value1, value_read1);

    const pps_value_t value_read2 = get_Htable_value(table, key2);

    add_Htable_value(table, key1, value3);

    const pps_value_t value_read3 = get_Htable_value(table, key1);
    const pps_value_t value_read4 = get_Htable_value(table, key3);

    // tests for input value = read value from get method
    ck_assert_str_eq(value2, value_read2);
    ck_assert_str_eq(value3, value_read3);
    ck_assert_ptr_null(value_read4);
    ck_assert_ptr_null(get_Htable_value(table, NULL));

    delete_Htable_and_content(&table);
}
END_TEST

/*UTILITARY FUNCTION TO PRINT THE CONTENT OF A BUCKET*/
void print_bucket(struct bucket* bck)
{
    if (bck == NULL) {
        printf("(null), ");
    } else {
        printf("kv(%s, %s), ", bck->key_value.key, bck->key_value.value);
        print_bucket(bck->next);
    }
}


/*UTILITARY FUNCTION TO PRINT A LIST OF NODES*/
void print_nodes(node_t* nodes, size_t size)
{
    char buffer[20];

    if (nodes == NULL)
        printf("THE REFERENCE IS NULL\n");

    printf("size_t = %lu\n", size);

    for (size_t i = 0; i < size; i++) {
        printf("node %lu address = %s port = %hu #nodes = %lu ", i, inet_ntop(AF_INET, &nodes[i].addr.sin_addr, buffer, 20), ntohs(nodes[i].addr.sin_port), nodes[i].id);
        printf("sha = ");
        print_sha(nodes[i].sha);
        printf("\n");
    }

}

START_TEST(get_nodes_test)
{
    printf("\nSTARTING TESTS FOR GET_NODES\n\n");
    node_list_t* nodes = get_nodes();

    if (nodes == NULL) {
        printf("get_nodes return NULL reference\n");
        return;
    }

    print_nodes(nodes->nodes, nodes->size);
}
END_TEST

START_TEST(get_hashtable_size)
{
//	printf("\nSTARTING TESTS FOR GET_HASHTABLE_CONTENT");

    Htable_t table = construct_Htable(5);

    // Defining tests ressource
    const pps_key_t key1 = "antoine";
    const pps_value_t value1 = "crettenand";
    const pps_key_t key2 = "julien";
    const pps_value_t value2 = "malka";

    add_Htable_value(table, key1, value1);
    add_Htable_value(table, key2, value2);

    /*Get the number of pairs in the htable*/
    size_t table_size = get_Htable_size(table);

    ck_assert_uint_eq(table_size, 2);

    add_Htable_value(table, "1", "a");
    add_Htable_value(table, "2", "b");
    add_Htable_value(table, "3", "c");

    table_size = get_Htable_size(table);

    ck_assert_uint_eq(table_size, 5);

}
END_TEST

/*UTILITARY FUNCTION TO PRINT THE CONTENT OF A KV_LIST*/
void print_kv_list(kv_list_t* list)
{
    printf("\n");
    for (int i = 0; i < list->size; i++) {
        printf("index %d => (%s, %s)\n", i, list->list[i].key, list->list[i].value);
    }
}

START_TEST(get_hashtable_content)
{
    printf("\nSTARTING TESTS FOR GET_HASHTABLE_CONTENT\n");

    Htable_t table = construct_Htable(5);

    // Defining tests ressource
    add_Htable_value(table, "1", "a");
    add_Htable_value(table, "2", "b");
    add_Htable_value(table, "3", "c");
    add_Htable_value(table, "4", "d");
    add_Htable_value(table, "5", "e");
    add_Htable_value(table, "6", "f");

    kv_list_t* list_of_kv = get_Htable_content(table);

    print_kv_list(list_of_kv);

    kv_list_free(list_of_kv);

}
END_TEST

START_TEST(parsing_argv)
{

    printf("STARTING TESTS FOR PARSING LINE COMMAND ARGUMENTS\n");

    char** array_of_str = calloc(7, sizeof(char*));
    array_of_str[0] = "-n";
    array_of_str[1] = "2";
    array_of_str[2] = "-w";
    array_of_str[3] = "3";
    array_of_str[4] = "--";
    array_of_str[5] = "antoine";
    array_of_str[6] = "crettenand";

    args_t* args = parse_opt_args(TOTAL_SERVERS | PUT_NEEDED, &array_of_str);

    printf("mandatory arguments = %s\n", *array_of_str);

    printf("results are N = %lu and W = %lu\n", args->N, args->W);
}
END_TEST

/*
START_TEST(node_list_sorting){

    printf("STARTING TESTS FOR SORTING NODE LIST\n");

    node_list_t *list_nodes = get_nodes();

    if (list_nodes == NULL) {
        printf("an error occurred in get_nodes");
        return;
    }

    printf("BEFORE SORTING : \n");
    print_nodes(list_nodes->nodes, list_nodes->size);

    node_list_sort(list_nodes, node_cmp_sha);

    printf("AFTER SORTING : \n");
    print_nodes(list_nodes->nodes, list_nodes->size);
}
END_TEST*/

START_TEST(ring_t_does_its_job)
{
    printf("\nSTARTING TESTS FOR RINGS\n");

    ring_t* ring = ring_alloc();
    ring_init(ring);

    print_nodes(ring->nodes, ring->size);
}
END_TEST

START_TEST(debug_print_ring_get_nodes_for_key)
{

    printf("\nSTARTING TESTS FOR RING_GET_NODES_FOR_KEY\n");

    ring_t* ring = ring_alloc();
    ring_init(ring);

    pps_key_t coucou = "bite1";

    printf("Segfault happens after\n");
    node_list_t* nodes_for_key = ring_get_nodes_for_key(ring, 3, coucou);
    printf("segfault\n");

    if (nodes_for_key == NULL) {
        printf("nodes_for_key is NULL\n");
        return;
    }
    if (nodes_for_key->nodes == NULL) printf("the list is null\n");
    printf("size of the nodes list = %lu\n", nodes_for_key->size);

//	char buffer[20];
//	printf("informations on first node = %s %hu %lu \n", inet_ntop(AF_INET, &nodes_for_key->nodes[0].addr.sin_addr, buffer, 20), ntohs(nodes_for_key->nodes[0].addr.sin_port), nodes_for_key->nodes[0].id);

    printf("nbr of key = %lu\n", nodes_for_key->size);

    print_nodes(nodes_for_key->nodes, nodes_for_key->size);

}
END_TEST

Suite *hashtable_suite()
{

    Suite *s = suite_create("hashtable.h");

    TCase *tc_ht = tcase_create("hashtable");
    suite_add_tcase(s, tc_ht);

    tcase_add_test(tc_ht, get_nodes_test);
    // tcase_add_test(tc_ht, add_get_hashtable);
    //  tcase_add_test(tc_ht, get_hashtable_size);
    //  tcase_add_test(tc_ht, get_hashtable_content);
    tcase_add_test(tc_ht, parsing_argv);
//    tcase_add_test(tc_ht, ring_t_does_its_job);
//    tcase_add_test(tc_ht, node_list_sorting);
    tcase_add_test(tc_ht, debug_print_ring_get_nodes_for_key);

    return s;
}

TEST_SUITE(hashtable_suite)
