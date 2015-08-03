#ifndef _commands_h_
#define _commands_h_

class IO_manager;
class Application;

class Command
{
protected:
	Application& m_app;

public:
	Command(Application& app) : m_app{app} {}
	virtual ~Command() = default;

	virtual bool is_persistent() = 0;
	virtual void print_prompt() const {}
	virtual void execute(const std::string& arg) = 0;
};

class Cmd_cd : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return true; }
	virtual void print_prompt() const;
	virtual void execute(const std::string& arg);
};

class Cmd_cc : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return true; }
	virtual void print_prompt() const;
	virtual void execute(const std::string& arg);
};

class Cmd_list : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string&);
};

class Cmd_column_list : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string&arg);
};

class Cmd_cursor_up : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_cursor_down : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_cursor_begin : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_cursor_end : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_cp : public Command
{
private:
	bool deref = false;
	bool update = false;

public:
	using Command::Command;

	virtual bool is_persistent() { return true; }
	virtual void print_prompt() const;
	virtual void execute(const std::string& arg);

private:
	bool settings(const char* arg);
};

class Cmd_mv : public Command
{
private:
	bool update = false;

public:
	using Command::Command;

	virtual bool is_persistent() { return true; }
	virtual void print_prompt() const;
	virtual void execute(const std::string& arg);

private:
	bool settings(const char* arg);
};

class Cmd_rm : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_io_pause : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_io_resume : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_io_cancel : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_create_tab : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_remove_tab : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_switch_tab : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

class Cmd_list_tabs : public Command
{
public:
	using Command::Command;

	virtual bool is_persistent() { return false; }
	virtual void execute(const std::string& arg);
};

#endif // _commands_h_
