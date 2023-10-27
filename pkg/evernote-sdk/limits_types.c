/**
 * Autogenerated by Thrift Compiler (0.14.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#include <math.h>

#include "limits_types.h"
#include <thrift/c_glib/thrift.h>

/* constants */
GHashTable *
edam_mime_types_constant (void)
{
  static GHashTable *constant = NULL;
  if (constant == NULL)
  {
    gchar * edam_mime_types0 = g_strdup ("image/gif");
    gchar * edam_mime_types1 = g_strdup ("image/jpeg");
    gchar * edam_mime_types2 = g_strdup ("image/png");
    gchar * edam_mime_types3 = g_strdup ("audio/wav");
    gchar * edam_mime_types4 = g_strdup ("audio/mpeg");
    gchar * edam_mime_types5 = g_strdup ("audio/amr");
    gchar * edam_mime_types6 = g_strdup ("application/vnd.evernote.ink");
    gchar * edam_mime_types7 = g_strdup ("application/pdf");
    gchar * edam_mime_types8 = g_strdup ("video/mp4");
    gchar * edam_mime_types9 = g_strdup ("audio/aac");
    gchar * edam_mime_types10 = g_strdup ("audio/mp4");

    constant = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    g_hash_table_insert (constant, edam_mime_types0, 0);
    g_hash_table_insert (constant, edam_mime_types1, 0);
    g_hash_table_insert (constant, edam_mime_types2, 0);
    g_hash_table_insert (constant, edam_mime_types3, 0);
    g_hash_table_insert (constant, edam_mime_types4, 0);
    g_hash_table_insert (constant, edam_mime_types5, 0);
    g_hash_table_insert (constant, edam_mime_types6, 0);
    g_hash_table_insert (constant, edam_mime_types7, 0);
    g_hash_table_insert (constant, edam_mime_types8, 0);
    g_hash_table_insert (constant, edam_mime_types9, 0);
    g_hash_table_insert (constant, edam_mime_types10, 0);
  }
  return constant;
}

GHashTable *
edam_indexable_resource_mime_types_constant (void)
{
  static GHashTable *constant = NULL;
  if (constant == NULL)
  {
    gchar * edam_indexable_resource_mime_types11 = g_strdup ("application/msword");
    gchar * edam_indexable_resource_mime_types12 = g_strdup ("application/mspowerpoint");
    gchar * edam_indexable_resource_mime_types13 = g_strdup ("application/excel");
    gchar * edam_indexable_resource_mime_types14 = g_strdup ("application/vnd.ms-word");
    gchar * edam_indexable_resource_mime_types15 = g_strdup ("application/vnd.ms-powerpoint");
    gchar * edam_indexable_resource_mime_types16 = g_strdup ("application/vnd.ms-excel");
    gchar * edam_indexable_resource_mime_types17 = g_strdup ("application/vnd.openxmlformats-officedocument.wordprocessingml.document");
    gchar * edam_indexable_resource_mime_types18 = g_strdup ("application/vnd.openxmlformats-officedocument.presentationml.presentation");
    gchar * edam_indexable_resource_mime_types19 = g_strdup ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
    gchar * edam_indexable_resource_mime_types20 = g_strdup ("application/vnd.apple.pages");
    gchar * edam_indexable_resource_mime_types21 = g_strdup ("application/vnd.apple.numbers");
    gchar * edam_indexable_resource_mime_types22 = g_strdup ("application/vnd.apple.keynote");
    gchar * edam_indexable_resource_mime_types23 = g_strdup ("application/x-iwork-pages-sffpages");
    gchar * edam_indexable_resource_mime_types24 = g_strdup ("application/x-iwork-numbers-sffnumbers");
    gchar * edam_indexable_resource_mime_types25 = g_strdup ("application/x-iwork-keynote-sffkey");

    constant = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types11, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types12, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types13, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types14, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types15, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types16, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types17, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types18, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types19, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types20, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types21, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types22, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types23, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types24, 0);
    g_hash_table_insert (constant, edam_indexable_resource_mime_types25, 0);
  }
  return constant;
}

GHashTable *
edam_indexable_plaintext_mime_types_constant (void)
{
  static GHashTable *constant = NULL;
  if (constant == NULL)
  {
    gchar * edam_indexable_plaintext_mime_types26 = g_strdup ("application/x-sh");
    gchar * edam_indexable_plaintext_mime_types27 = g_strdup ("application/x-bsh");
    gchar * edam_indexable_plaintext_mime_types28 = g_strdup ("application/sql");
    gchar * edam_indexable_plaintext_mime_types29 = g_strdup ("application/x-sql");

    constant = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    g_hash_table_insert (constant, edam_indexable_plaintext_mime_types26, 0);
    g_hash_table_insert (constant, edam_indexable_plaintext_mime_types27, 0);
    g_hash_table_insert (constant, edam_indexable_plaintext_mime_types28, 0);
    g_hash_table_insert (constant, edam_indexable_plaintext_mime_types29, 0);
  }
  return constant;
}

GHashTable *
edam_publishing_uri_prohibited_constant (void)
{
  static GHashTable *constant = NULL;
  if (constant == NULL)
  {
    gchar * edam_publishing_uri_prohibited30 = g_strdup (".");
    gchar * edam_publishing_uri_prohibited31 = g_strdup ("..");

    constant = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, NULL);
    g_hash_table_insert (constant, edam_publishing_uri_prohibited30, 0);
    g_hash_table_insert (constant, edam_publishing_uri_prohibited31, 0);
  }
  return constant;
}

