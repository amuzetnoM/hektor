"""
Hektor Vector Database - High-performance vector database with SIMD-optimized similarity search.

Author: Ali A. Shakil
Organization: ARTIFACT VIRTUAL
License: MIT
"""

__version__ = "4.1.0"
__author__ = "Ali A. Shakil"
__email__ = "ali.shakil@artifactvirtual.com"

# Import the native extension
try:
    from .pyvdb import *
    _NATIVE_AVAILABLE = True
except ImportError as e:
    _NATIVE_AVAILABLE = False
    _IMPORT_ERROR = str(e)
    
    # Provide helpful error message
    import warnings
    warnings.warn(
        f"Native pyvdb extension not available: {_IMPORT_ERROR}. "
        "Some features may be limited. Try reinstalling with: pip install --force-reinstall hektor-vdb"
    )

def is_native_available() -> bool:
    """Check if the native C++ extension is available."""
    return _NATIVE_AVAILABLE

# Re-export version
VERSION = __version__
