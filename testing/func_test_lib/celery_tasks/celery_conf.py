from celery import Celery
from datetime import datetime, timedelta

REDIS = 'redis://localhost:6379/11'

test_app = Celery('test_app',
    broker=REDIS,
    include=['func_test_lib.celery_tasks.tasks']
)

test_app.conf.update(
    BROKER_TRANSPORT_OPTIONS={'visibility_timeout': 60*5},
    CELERY_RESULT_BACKEND=REDIS,
    CELERY_TASK_RESULT_EXPIRES=timedelta(minutes=10),
    CELERYD_TASK_SOFT_TIME_LIMIT=60*3,
    CELERYD_TASK_TIME_LIMIT=60*5,
    CELERYD_MAX_TASKS_PER_CHILD=20,
    CELERY_CREATE_MISSING_QUEUES=True,
    CELERY_RESULT_PERSISTENT=True,
    CELERY_TASK_SERIALIZER='pickle',
    CELERY_ACCEPT_CONTENT=['pickle', 'json'],
    CELERY_SEND_TASK_SENT_EVENT=True
    # CELERYD_HIJACK_ROOT_LOGGER = False,
)
