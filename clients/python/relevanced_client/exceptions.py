class RelevancedClientError(Exception):
    pass

class ClassifierDoesNotExist(RelevancedClientError):
    pass

class ClassifierAlreadyExists(RelevancedClientError):
    pass

class DocumentDoesNotExist(RelevancedClientError):
    pass

class DocumentAlreadyExists(RelevancedClientError):
    pass

class UnexpectedResponse(RelevancedClientError):
    pass
