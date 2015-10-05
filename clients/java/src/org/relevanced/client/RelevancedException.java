package org.relevanced.client.exceptions;

public class RelevancedException extends Exception {
    public RelevancedException() {}
    public RelevancedException(String message) {
        super(message);
    }
    public RelevancedException(Throwable cause) {
        super(cause);
    }
    public RelevancedException(String message, Throwable cause) {
        super(message, cause);
    }
    public RelevancedException(String message, Throwable cause, boolean enableSuppression, boolean writableStackTrace) {
        super(message, cause, enableSuppression, writableStackTrace);
    }
}

public class CentroidDoesNotExist extends RelevancedException {}
public class DocumentDoesNotExist extends RelevancedException {}
public class CentroidAlreadyExists extends RelevancedException {}
public class DocumentAlreadyExists extends RelevancedException {}
public class UnspecifiedException extends RelevancedException {}




