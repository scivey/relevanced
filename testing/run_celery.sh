#!/bin/bash

celery worker -A func_test_lib.celery_tasks.celery_conf --loglevel INFO
