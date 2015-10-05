package org.relevanced.client.exceptions;

public class ConnectionError extends RelevancedException {
    public ConnectionError(String message) {
        super(message);
    }
    public ConnectionError(Throwable cause) {
        super(cause);
    }
}
