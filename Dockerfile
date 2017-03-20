FROM eabatalov/aucont-test-base
USER 0
RUN apt-get install -y cgroup-tools
USER 1000
