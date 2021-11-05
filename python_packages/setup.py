from setuptools import setup, find_packages

setup(
    packages=find_packages(),
    name="dvbcdr",
    version="0.0.1-alpha",
    install_requires=["numpy", "pyserial", "pyyaml"]
)
