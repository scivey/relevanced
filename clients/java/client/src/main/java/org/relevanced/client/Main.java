package org.relevanced.client;

import java.lang.System;

public class Main {
    public static void main(String [] args) {
        System.out.println("start");
        RelevancedBlockingClient client = new RelevancedBlockingClient("localhost", 8097);
        System.out.println("end");
    }
}