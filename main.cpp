#include <iostream>
#include <memory>
#include <exception>
#include <clocale>
#include <unordered_map>
#include <memory>
#include "Application.h"
#include "commands.h"

using namespace std;
using namespace hawk;

using Cmd_map = unordered_map<string, unique_ptr<Command>>;

Cmd_map create_commands(Application& app)
{
	Cmd_map cmds;

	// path control
	cmds["."] = make_unique<Cmd_cd>(app);
	cmds[","] = make_unique<Cmd_cc>(app);
	cmds["+"] = make_unique<Cmd_list>(app);
	cmds["++"] = make_unique<Cmd_column_list>(app);

	// cursor control
	cmds["~"] = make_unique<Cmd_cursor_up>(app);
	cmds["`"] = make_unique<Cmd_cursor_down>(app);
	cmds["~~"] = make_unique<Cmd_cursor_begin>(app);
	cmds["``"] = make_unique<Cmd_cursor_end>(app);

	// I/O control
	cmds[">"] = make_unique<Cmd_cp>(app);
	cmds[">>"] = make_unique<Cmd_mv>(app);
	cmds[">>>"] = make_unique<Cmd_rm>(app);

	cmds["<"] = make_unique<Cmd_io_pause>(app);
	cmds["<<"] = make_unique<Cmd_io_resume>(app);
	cmds["<<<"] = make_unique<Cmd_io_cancel>(app);

	// tab control
	cmds["|>"] = make_unique<Cmd_create_tab>(app);
	cmds["|<"] = make_unique<Cmd_remove_tab>(app);
	cmds["|"] = make_unique<Cmd_switch_tab>(app);
	cmds["||"] = make_unique<Cmd_list_tabs>(app);

	return cmds;
}

void parse_cmd(const string& in, string& cmd, string& arg)
{
	constexpr char arg_separator = ':';

	auto pos = in.find(arg_separator);
	if (pos != string::npos)
	{
		arg = in.c_str() + pos + 1;
		cmd = in.substr(0, pos);
	}
	else
		cmd = in;
}

Command* get_cmd_handler(const Cmd_map& cmds, const string& cmd)
{
	auto it = cmds.find(cmd);
	return (it != cmds.end()) ? it->second.get() : nullptr;
}

bool execute_new_cmd(const Cmd_map& cmds, Command*& pcmd, const string& in)
{
	string cmd_str;
	string arg;

	parse_cmd(in, cmd_str, arg);
	Command* cmd = get_cmd_handler(cmds, cmd_str);

	if (!cmd)
	{
		// no command handler found, continue with current persistent cmd
		return false;
	}

	if (cmd->is_persistent() && arg.empty())
	{
		pcmd = cmd;
		return true;
	}

	cmd->execute(arg);

	return true;
}

int main()
{
	setlocale(LC_ALL, "");

	Application app {"/"};
	Cmd_map cmds = create_commands(app);

	string in;
	Command* persistent_cmd = cmds["."].get();

	for (;;)
	{
		persistent_cmd->print_prompt();
		getline(cin, in);

		if (in.empty())
			continue;

		if (in == "\\")
			break;

		try {
			if (!execute_new_cmd(cmds, persistent_cmd, in))
				persistent_cmd->execute(in);
		} catch (const invalid_argument& e) {
			cerr << "# Invalid argument: " << e.what() << endl;
		} catch (const out_of_range& e) {
			cerr << "# Out of range: " << e.what() << endl;
		} catch (const Filesystem_error& e) {
			cerr << "# Filesystem error: " << e.what() << endl;
		}

		in.clear();
	}

	cout << "bye..." << endl;

	return 0;
}
