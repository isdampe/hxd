import os
import ycm_core

flags = [
  '-Wall',
  '-Wextra',
  '-Werror',
  '-std=c99',
  ]

SOURCE_EXTENSIONS = [ '.c' ]

def FlagsForFile( filename, **kwargs ):
  return {
  'flags': flags,
  'do_cache': True
  }
