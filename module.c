#include<module.h>
#include<mem.h>
#include<generatorConfig.h>
#include"lib_generator.h"

extern void elog_debug(char *fmt, ...);

static void generator_modules_cb(module_ctx *mod)
{
  PGconn *conn = module_get_pgconn(mod);
  char *error = 0;
  WJElement params = module_get_params(mod);
  char *userid;

  userid = module_get_user(mod);

  elog_debug("%s", __func__);

  generator_modules(conn, userid, &error);
  if(error)
    {
      module_error(mod, MODERR_API, error);
      return;
    }
  module_connection_ready(mod, module_generic_dbreply);
}
void mod_generator_modules(module_ctx *mod)
{
  module_connection_get(mod, generator_modules_cb, 0);
}
static void generator_module_cb(module_ctx *mod)
{
  PGconn *conn = module_get_pgconn(mod);
  char *error = 0;
  int restno, restlen;
  restful_t *r = module_restful(mod);
  char *id;

  restno = restful_gettemplate_param(r, "id");
  restlen = r->params[restno].end-r->params[restno].begin; 
  id = mem_calloc(1, restlen+1);
  snprintf(id, restlen+1, "%.*s", 
	   r->params[restno].end-r->params[restno].begin, 
	   r->path+r->params[restno].begin);

  elog_debug("%s", __func__);

  generator_module(conn, id, &error);
  if(error)
    {
      module_error(mod, MODERR_API, error);
      return;
    }
  module_connection_ready(mod, module_generic_dbreply);
  mem_free(id);
}
void mod_generator_module(module_ctx *mod)
{
  module_connection_get(mod, generator_module_cb, 0);
}

extern void generator_getfiles(module_ctx *mod);
void mod_generator_files(module_ctx *mod)
{
  generator_getfiles(mod);
}

extern void generator_getfile(module_ctx *mod);
void mod_generator_file(module_ctx *mod)
{
  generator_getfile(mod);
}

static void generator_moduleinfo_cb(module_ctx *mod)
{
  PGconn *conn = module_get_pgconn(mod);
  char *error = 0;
  int restno, restlen;
  restful_t *r = module_restful(mod);
  char *id;

  restno = restful_gettemplate_param(r, "id");
  restlen = r->params[restno].end-r->params[restno].begin; 
  id = mem_calloc(1, restlen+1);
  snprintf(id, restlen+1, "%.*s", 
	   r->params[restno].end-r->params[restno].begin, 
	   r->path+r->params[restno].begin);

  elog_debug("%s", __func__);

  generator_moduleinfo(conn, id, &error);
  if(error)
    {
      module_error(mod, MODERR_API, error);
      return;
    }
  module_connection_ready(mod, module_generic_dbreply);
  mem_free(id);
}
void mod_generator_moduleinfo(module_ctx *mod)
{
  module_connection_get(mod, generator_moduleinfo_cb, 0);
}
extern void generator_newmodule_reply(module_ctx*);
static void generator_newmodule_cb(module_ctx *mod)
{
  PGconn *conn = module_get_pgconn(mod);
  char *error = 0;
  WJElement params = module_get_params(mod);
  char *module;

  module = WJEToString(WJEChild(params, "module", WJE_GET), 0);

  elog_debug("%s module=%s", __func__, module);

  generator_newmodule(conn, module, &error);
  if(error)
    {
      module_error(mod, MODERR_API, error);
      return;
    }
  module_connection_ready(mod, generator_newmodule_reply);
  MemFree(module);
}

void mod_generator_newmodule(module_ctx *mod)
{
  WJElement params = module_get_params(mod);
  const char *jschema="{"
    "\"$schema\": \"http://json-schema.org/draft-04/schema#\","
    "\"title\": \"newmodule schema\","
    "\"type\": \"object\","
    "\"properties\": {"
    "\"module\":{"

     "\"type\": \"object\","
         "\"properties\": {"
         "\"name\":{"
         "\"type\":\"string\""
         "}},"

         "\"required\": ["
         "\"name\"]"

    "}},"
    "\"required\": ["
    "\"module\"]"
    "}";

  WJElement schema = WJEParse(jschema);
  if(!WJESchemaValidate(schema, params, 0, 0, 0, 0))
    {
      module_error(mod, MODERR_RESTFUL,
		   code_str(standardErrors, RESTFUL_PARAMS_ERR));
      return;
    }
  module_connection_get(mod, generator_newmodule_cb, 0);
  WJECloseDocument(schema);
}

void mod_generator_generate(module_ctx *mod)
{
  WJElement params = module_get_params(mod);

  const char *jschema="{"
    "\"$schema\": \"http://json-schema.org/draft-04/schema#\","
    "\"title\": \"generate schema\","
    "\"type\": \"object\","
    "\"properties\": {"
    "\"module\":{"
    "\"type\":\"string\""
    "},"
    "\"sections\":{"
    "\"type\":\"string\""
    "}},"
    "\"required\": ["
    "\"module\",\"sections\"]"
    "}";

  WJElement schema = WJEParse(jschema);
  if(!WJESchemaValidate(schema, params, 0, 0, 0, 0))
    {
      module_error(mod, MODERR_RESTFUL,
		   code_str(standardErrors, RESTFUL_PARAMS_ERR));
      return;
    }
    generator_generate_cb(mod);
}

void mod_generator_upload(module_ctx *mod)
{
  WJElement params = module_get_params(mod);

  const char *jschema="{"
    "\"$schema\": \"http://json-schema.org/draft-04/schema#\","
    "\"title\": \"upload schema\","
    "\"type\": \"object\","
    "\"properties\": {"
    "\"module\":{"
    "\"type\":\"string\""
    "},"
    "\"section\":{"
    "\"type\":\"string\""
    "},"
    "\"data\":{"
    "\"type\":\"string\""
    "},"
      "\"name\":{"
      "\"type\":\"string\""
      "}},"
    "\"required\": ["
    "\"module\",\"section\",\"data\",\"name\"]"
    "}";

  WJElement schema = WJEParse(jschema);
  if(!WJESchemaValidate(schema, params, 0, 0, 0, 0))
    {
      module_error(mod, MODERR_RESTFUL,
		   code_str(standardErrors, RESTFUL_PARAMS_ERR));
      return;
    }

  generator_upload(mod);
}

#ifndef FASTCALL_DISABLE
static void __attribute__((constructor)) generator_init(void);
static void __attribute__((destructor)) generator_fini(void);
#endif

static void generator_init(void)
{
  module_symbol_register("generator_modules", mod_generator_modules);
  module_symbol_register("generator_module", mod_generator_module);
  module_symbol_register("generator_files", mod_generator_files);
  module_symbol_register("generator_file", mod_generator_file);
  module_symbol_register("generator_moduleinfo", mod_generator_moduleinfo);
  module_symbol_register("generator_newmodule", mod_generator_newmodule);
  module_symbol_register("generator_generate", mod_generator_generate);
  module_symbol_register("generator_upload", mod_generator_upload);
}

static void generator_fini(void)
{
  module_symbol_unregister("generator_modules");
  module_symbol_unregister("generator_module");
  module_symbol_unregister("generator_files");
  module_symbol_unregister("generator_file");
  module_symbol_unregister("generator_moduleinfo");
  module_symbol_unregister("generator_newmodule");
  module_symbol_unregister("generator_generate");
  module_symbol_unregister("generator_upload");
}
