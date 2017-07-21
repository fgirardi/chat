project('chat', 'cpp',
	 default_options: ['cpp_std=c++11'])
add_global_arguments('-DENABLE_NCURSES', language: 'cpp')
dep_ncurses = dependency('ncurses')
dep_threads = dependency('threads')

executable('server', ['src/server.cxx', 'src/screen.cxx']
		, dependencies: [dep_ncurses, dep_threads])
executable('client', ['src/client.cxx', 'src/screen.cxx']
		, dependencies: [dep_ncurses, dep_threads])
