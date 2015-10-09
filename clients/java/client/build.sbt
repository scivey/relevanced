name := "client"

// orgnization name (e.g., the package name of the project)
organization := "org.relevanced"

version := "0.8.0a3-SNAPSHOT"

// project description
description := "Java client for relevanced text similarity server."

// Enables publishing to maven repo
publishMavenStyle := true

// Do not append Scala versions to the generated artifacts
crossPaths := false

// This forbids including Scala related libraries into the dependency
autoScalaLibrary := false
sourceDirectories in Compile += file("src")
mainClass in (Compile, run) := Some("org.relevanced.client.Main")

// library dependencies. (org name) % (project name) % (version)
libraryDependencies ++= Seq(
   "org.apache.thrift" % "libthrift" % "0.9.2",
   "org.slf4j"         % "slf4j-jdk14" % "1.7.12"
)
