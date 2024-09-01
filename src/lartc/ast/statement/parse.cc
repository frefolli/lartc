#include <lartc/ast/statement/parse.hh>
#include <lartc/ast/type/parse.hh>
#include <lartc/ast/expression/parse.hh>
#include <lartc/ast/parse.hh>
#include <lartc/internal_errors.hh>
#include <lartc/tree_sitter.hh>
#include <cstring>
#include <tree_sitter/api.h>
#include <unordered_map>
#include <iostream>

inline Statement* parse_statement_block(TSContext& context, TSNode& node) {
  Statement* block = Statement::New(statement_t::BLOCK_STMT);

  uint64_t child_count = ts_node_named_child_count(node);
  for (uint64_t child_index = 0; child_index < child_count; ++child_index) {
    TSNode child_node = ts_node_named_child(node, child_index);
    Statement* child_stmt = parse_statement(context, child_node);
    if (child_stmt != nullptr) {
      block->children.push_back(child_stmt);
    } else {
      const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(child_node));
      if (!ts_can_ignore(symbol_name)) {
        throw_internal_error(UNHANDLED_TS_SYMBOL_NAME, MSG(": " << std::string(symbol_name) << " inside a (block)"));
      }
    }
  }

  return block;
}

inline Statement* parse_statement_stub(TSContext& context, TSNode& node) {
  std::clog << TS_DEBUG << " => " << ts_node_string(node) << std::endl;
  return nullptr;
}

inline Statement* parse_statement_let(TSContext& context, TSNode& node) {
  Statement* let = Statement::New(statement_t::LET_STMT);
  
  TSNode name = ts_node_child_by_field_name(node, "name");
  let->name = ts_node_source_code(name, context.source_code);

  TSNode type = ts_node_child_by_field_name(node, "type");
  let->type = parse_type(context, type);
  ts_validate_parsing(context.language, type, "let:type", let->type);

  return let;
}

inline Statement* parse_statement_return(TSContext& context, TSNode& /*node*/) {
  Statement* return_ = Statement::New(statement_t::RETURN_STMT);
  return return_;
}

inline Statement* parse_statement_break(TSContext& context, TSNode& /*node*/) {
  Statement* break_ = Statement::New(statement_t::BREAK_STMT);
  return break_;
}

inline Statement* parse_statement_continue(TSContext& context, TSNode& /*node*/) {
  Statement* continue_ = Statement::New(statement_t::CONTINUE_STMT);
  return continue_;
}

inline Statement* parse_statement_if_else(TSContext& context, TSNode& node) {
  Statement* if_else = Statement::New(statement_t::IF_ELSE_STMT);

  TSNode condition = ts_node_child_by_field_name(node, "condition");
  if_else->condition = parse_expression(context, condition);
  ts_validate_parsing(context.language, condition, "for:condition", if_else->condition);

  TSNode then = ts_node_child_by_field_name(node, "then");
  if_else->then = parse_statement(context, then);
  ts_validate_parsing(context.language, then, "if_else:then", if_else->then);

  TSNode else_ = ts_node_child_by_field_name(node, "else");
  if (else_.id) {
    if_else->else_ = parse_statement(context, else_);
    ts_validate_parsing(context.language, else_, "if_else:else", if_else->else_);
  }

  return if_else;
}

inline Statement* parse_statement_for(TSContext& context, TSNode& node) {
  Statement* for_ = Statement::New(statement_t::FOR_STMT);

  TSNode init = ts_node_child_by_field_name(node, "init");
  for_->init = parse_statement(context, init);
  ts_validate_parsing(context.language, init, "for:init", for_->init);

  TSNode condition = ts_node_child_by_field_name(node, "condition");
  for_->condition = parse_expression(context, condition);
  ts_validate_parsing(context.language, condition, "for:condition", for_->condition);

  TSNode step = ts_node_child_by_field_name(node, "step");
  for_->step = parse_expression(context, step);
  ts_validate_parsing(context.language, step, "for:step", for_->step);

  TSNode body = ts_node_child_by_field_name(node, "body");
  for_->body = parse_statement(context, body);
  ts_validate_parsing(context.language, body, "for:body", for_->body);

  return for_;
}

inline Statement* parse_statement_while(TSContext& context, TSNode& node) {
  Statement* while_ = Statement::New(statement_t::WHILE_STMT);

  TSNode body = ts_node_child_by_field_name(node, "body");
  while_->body = parse_statement(context, body);
  ts_validate_parsing(context.language, body, "while:body", while_->body);

  return while_;
}

typedef Statement*(*statement_parser)(TSContext& context, TSNode& node);
std::unordered_map<std::string, statement_parser> statement_parsers = {
  {"let_statement", parse_statement_let},
  {"const_statement", parse_statement_let}, // TODO: refactor for const types and/or remove const declaration
  {"return_statement", parse_statement_return},
  {"break_statement", parse_statement_break},
  {"continue_statement", parse_statement_continue},
  {"if_else", parse_statement_if_else},
  {"while", parse_statement_while},
  {"for", parse_statement_for},
  {"block", parse_statement_block},
};

Statement* parse_statement(TSContext& context, TSNode& node) {
  const char* symbol_name = ts_language_symbol_name(context.language, ts_node_grammar_symbol(node));
  auto it = statement_parsers.find(symbol_name);
  if (it != statement_parsers.end()) {
    return it->second(context, node);
  } else {
    Expression* expr = parse_expression(context, node);
    if (expr != nullptr) {
      Statement* stmt = Statement::New(statement_t::EXPRESSION_STMT);
      stmt->expr = expr;
      return stmt;
    }
  }
  return nullptr;
}
