/*
 * Copyright (c) 2003-2009, John Wiegley.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of New Artisans LLC nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   global.h
 * @author John Wiegley
 *
 * @brief Contains the top-level functions used by main.cc
 */
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "report.h"

namespace ledger {

class global_scope_t : public noncopyable, public scope_t
{
  scoped_ptr<session_t> session_ptr;
  ptr_list<report_t>	report_stack;

public:
  path script_file;

  global_scope_t(char ** envp);
  ~global_scope_t();

  void read_journal_files();

  char * prompt_string();

  session_t& session() {
    return *session_ptr.get();
  }
  report_t& report() {
    return report_stack.front();
  }

  void push_report() {
    report_stack.push_front(new report_t(report_stack.front()));
  }
  void pop_report() {
    if (! report_stack.empty())
      report_stack.pop_front();
  }

  void report_error(const std::exception& err);

  /**
   * @return \c true if a command was actually executed; otherwise, it probably
   *         just resulted in setting some options.
   */
  void execute_command(strings_list args, bool at_repl);
  int  execute_command_wrapper(strings_list args, bool at_repl);

  value_t push_report_cmd(call_scope_t&) {
    // Make a copy at position 2, because the topmost report object has an
    // open output stream at this point.  We want it to get popped off as
    // soon as this command terminate so that the stream is closed cleanly.
    report_stack.insert(++report_stack.begin(),
			new report_t(report_stack.front()));
    return true;
  }
  value_t pop_report_cmd(call_scope_t&) {
    pop_report();
    return true;
  }

  value_t option_script_(call_scope_t& args) {
    script_file = args[0].as_string();
    return true;
  }

  value_t ignore(call_scope_t&) {
    return true;
  }

  virtual expr_t::ptr_op_t lookup(const string& name);
};

void         handle_debug_options(int argc, char * argv[]);
void         read_environment_settings(report_t& report, char * envp[]);
strings_list read_command_arguments(scope_t& scope, strings_list args);
void         normalize_session_options(session_t& session);
function_t   look_for_precommand(scope_t& scope, const string& verb);
function_t   look_for_command(scope_t& scope, const string& verb);
void         normalize_report_options(report_t& report, const string& verb);

} // namespace ledger

#endif // _GLOBAL_H