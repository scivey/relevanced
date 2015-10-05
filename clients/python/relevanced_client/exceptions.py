from __future__ import absolute_import

class RelevancedClientError(Exception):
    pass

class CentroidDoesNotExist(RelevancedClientError):
    pass

class CentroidAlreadyExists(RelevancedClientError):
    pass

class DocumentDoesNotExist(RelevancedClientError):
    pass

class DocumentAlreadyExists(RelevancedClientError):
    pass

class UnexpectedResponse(RelevancedClientError):
    pass
