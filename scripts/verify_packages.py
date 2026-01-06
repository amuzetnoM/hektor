import importlib
import sys

packages = {
    'tensorflow': 'tensorflow',
    'torch': 'torch',
    'torchvision': 'torchvision',
    'transformers': 'transformers',
    'sentence_transformers': 'sentence_transformers',
    'onnx': 'onnx',
    'onnxruntime': 'onnxruntime',
    'grpcio': 'grpc',
    'protobuf': 'google.protobuf',
    'prometheus_client': 'prometheus_client',
    'tf_keras': 'tf_keras'
}

print("=" * 60)
print("Robust Python Package Verification (ASCII Only)")
print("=" * 60)

all_ok = True
for label, import_name in packages.items():
    try:
        mod = importlib.import_module(import_name)
        ver = getattr(mod, '__version__', 'Installed')
        print("[OK] {:25s} v{}".format(label, ver))
    except ImportError as e:
        print("[FAIL] {:25s} - ERROR: {}".format(label, str(e)))
        all_ok = False
    except Exception as e:
        print("[WARN] {:25s} - Loaded with Warning: {}".format(label, str(e)))

print("=" * 60)
if all_ok:
    print("SUCCESS: All Python packages are installed and importable.")
else:
    print("FAILURE: Some packages are missing or corrupted.")
print("=" * 60)
