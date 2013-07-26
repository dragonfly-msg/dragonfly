Instructions for Linux (see the wiki for further information):

Basic setup is performed by running make in the Python directory. If you have added new MDF structures, these need to create ctypes versions of the structure. Currently all MDFs in Dragonfly_types.h are present in Dragonfly_types.py, thanks to automatic code generators h2xml.py and xml2py.py. Possible options are:

    * adding Dragonfly_types.py manually, following the structure shown therein
    * add your MDF to a C header file Dragonfly_types.h and (re-)generating an associated Python version, like Dragonfly_types.py
    * add the type to your Python module directly 


