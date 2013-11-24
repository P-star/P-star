#pragma once

#ifndef  WIN32

class wpl_namespace;

class wpl_module {
	private:
	public:
	virtual void export_identifiers (wpl_namespace *target_namespace) = 0;
};

extern "C" {
	int wpl_module_init(int argc, char **argv);
	void wpl_module_exit();
	wpl_namespace *wpl_module_get_namespace();
}

#endif
