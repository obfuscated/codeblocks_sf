#
# Builds and runs tests for wxPropertyGrid
#

import sys, os, os.path

from shutil import copyfile


COMPILER_PREPARE_SCRIPTS = {
#'VC6' : 'prepare_vc6.bat',
'VC7' : 'prepare_vc7.bat',
'VC8' : 'prepare_vc8.bat',
'VC9' : 'prepare_vc9.bat',
}

RUN_LIST = [
#('2.6.4-VC6',
#    'VC6', r'C:\lib\wxWidgets-2.6.4',
#    'WX_UNICODE=1 WX_DEBUG=0 WX_SHARED=0'),

('2.8-SVN-VC9',
    'VC9', r'C:\lib\wxWidgets_SVN_28',
    'WX_UNICODE=1 WX_DEBUG=1 WX_SHARED=0'),

('2.8-SVN-VC9-R',
    'VC9', r'C:\lib\wxWidgets_SVN_28',
    'WX_UNICODE=1 WX_DEBUG=0 WX_SHARED=0'),

('2.8-SVN-VC9-DLL-R',
    'VC9', r'C:\lib\wxWidgets_SVN_28',
    'WX_UNICODE=1 WX_DEBUG=0 WX_SHARED=1'),
]

def print_help():
    print('Available options:')
    print('')
    print('  --only=X       Only do build X')
    print('  --list         List all possible builds')
    print('  --no-test      Just build the sample, don\'t run tests')

def main():
    cwd = os.getcwd()
    comp_src = cwd

    only_wxversion = None
    resume_from_wxversion = None
    resumed = True
    no_test = False

    for arg in sys.argv[1:]:
        if arg == '--help':
            print_help()
            return
        elif arg == '--list':
            print('Available builds:')
            print('')
            print('  Name                Compiler  Build Options')
            for build_name, compiler_name, wxdir, build_opts in RUN_LIST:
                print('  %-20s%-10s%s' % \
                      (build_name, compiler_name, build_opts))
            return
        elif arg.startswith('--only'):
            only_wxversion = arg.split('=',1)[1].strip()
        elif arg.startswith('--resume-from'):
            resume_from_wxversion = arg.split('=',1)[1].strip()
        elif arg == '--no-test':
            no_test = True
        else:
            resume_from_wxversion = arg

    if resume_from_wxversion:
        resumed = False

    # Check dirs
    for build_name, compiler_name, wxdir, build_opts in RUN_LIST:
        if not os.path.isdir(wxdir):
            print '-- No such path: %s'%wxdir
            return

    for build_name, compiler_name, wxdir, build_opts in RUN_LIST:

        if only_wxversion and not (only_wxversion == build_name):
            continue

        if not resumed:
            if resume_from_wxversion and resume_from_wxversion == build_name:
                resumed = True
            else:
                continue

        print('')
        print('Building for \'wxWidgets %s - %s\'' % \
              (build_name, build_opts))

        is_shared = 'WX_SHARED=1' in build_opts
        old_path = os.environ['PATH']

        if is_shared:
            # Add vc_dll dirs to the path
            new_path = '%s;%s;%s' % \
                    (os.path.join(wxdir, 'lib', 'vc_dll'),
                     os.path.join(cwd, 'lib', 'vc_dll'),
                     old_path)
            os.environ['PATH'] = new_path

        if build_name.startswith('2.6'):
            build_subdir = 'build_wx26'
        elif build_name.startswith('2.8'):
            build_subdir = 'build'
        else:
            build_subdir = 'build_wx29'

        build_path = os.path.join(comp_src, build_subdir)
        sample_path = os.path.join(comp_src, 'samples')

        os.chdir(build_path)

        build_clean_cmdline = 'nmake -f makefile.vc clean %s'%(build_opts)
        build_cmdline = 'nmake -f makefile.vc %s'%(build_opts)

        temp_builder_fn = '__temp_builder__.bat'

        f = file(temp_builder_fn,'wt')
        f.write('@echo off\n')

        prepare_script = COMPILER_PREPARE_SCRIPTS[compiler_name]
        f.write('call %s\n'%prepare_script)
        f.write('set WXWIN=%s\n'%wxdir)
        f.write('%s\n'%build_clean_cmdline)
        f.write('%s\n'%build_cmdline)
        f.close()

        res = os.system(temp_builder_fn)
        os.remove(temp_builder_fn)
        if res:
            print '-- \'%s\' failed --'%build_cmdline
            return

        os.chdir(sample_path)

        # Copy sample app for further testing
        copyfile('propgridsample.exe',
                 'propgridsample (%s %s).exe'%(build_name, build_opts))
        copyfile('propgridsample.exe.manifest',
                 'propgridsample (%s %s).exe.manifest' % \
                    (build_name, build_opts))
        if not no_test:
            res = os.system('propgridsample.exe --run-tests')
        os.environ['PATH'] = old_path

if __name__ == '__main__':
    main()
