from setuptools import find_packages, setup


def read(path):
    with open(path, "r") as f:
        return f.read()


long_description = read("README.md")

setup(
    name="mx",
    version="0.0.0",
    url="https://github.com/sbooeshaghi/mx",
    author="Sina Booeshaghi",
    author_email="alisina@caltech.edu",
    maintainer="Sina Booeshaghi",
    maintainer_email="alisina@caltech.edu",
    description="",  # noqa
    long_description=long_description,
    long_description_content_type="text/markdown",
    keywords="",
    python_requires=">=3.7",
    license="MIT",
    packages=find_packages(exclude=("tests", "tests.*")),
    zip_safe=False,
    include_package_data=True,
    install_requires=read("requirements.txt").strip().split("\n"),
    entry_points={
        "console_scripts": ["mx=mx.main:main"],
    },
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "Environment :: Console",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
        "Programming Language :: Python :: 3.7",
        "Topic :: Scientific/Engineering :: Bio-Informatics",
        "Topic :: Utilities",
    ],
)
