from setuptools import setup

version_path = 'relevanced_client/version.py'

exec(open(version_path).read())

setup(
    name="relevanced-client",
    zip_safe=True,
    version=VERSION,
    description="Python client for relevanced-server.",
    url="http://www.relevanced.org",
    maintainer="Scott Ivey",
    maintainer_email="scott.ivey@gmail.com",
    packages=['relevanced_client', 'relevanced_client.gen_py', 'relevanced_client.gen_py.RelevancedProtocol'],
    package_dir={'relevanced_client': 'relevanced_client'},
    install_requires=['thrift==0.9.2']
)