name := "client"

// orgnization name (e.g., the package name of the project)
organization := "org.relevanced"

version := "0.9.3-SNAPSHOT"

// project description
description := "Java client for relevanced text similarity server."

// Enables publishing to maven repo
publishMavenStyle := true

// Do not append Scala versions to the generated artifacts
crossPaths := false

// This forbids including Scala related libraries into the dependency
autoScalaLibrary := false
sourceDirectories in Compile += file("src")
// mainClass in (Compile, run) := Some("org.relevanced.client.Main")

// library dependencies. (org name) % (project name) % (version)
libraryDependencies ++= Seq(
   "org.apache.thrift" % "libthrift" % "0.9.2",
   "org.slf4j"         % "slf4j-jdk14" % "1.7.12"
)

publishTo := {
  val nexus = "https://oss.sonatype.org/"
  if (isSnapshot.value)
    Some("snapshots" at nexus + "content/repositories/snapshots")
  else
    Some("releases" at nexus + "service/local/staging/reploy/maven2")
}

pomExtra := (
  <url>http://www.relevanced.org</url>
  <licenses>
    <license>
      <name>MIT</name>
      <url>http://www.opensource.org/licenses/mit-license.php</url>
      <distribution>repo</distribution>
    </license>
  </licenses>
  <scm>
    <url>git@github.com:scivey/relevanced.git</url>
    <connection>scm:git:git@github.com:scivey/relevanced.git</connection>
  </scm>
  <developers>
    <developer>
      <id>scivey</id>
      <name>Scott Ivey</name>
      <url>https://github.com/scivey</url>
    </developer>
  </developers>)
