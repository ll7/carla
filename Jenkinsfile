pipeline {
    agent any

    environment {
        UE4_ROOT = '/var/lib/jenkins/UnrealEngine_4.22'
    }

    options {
        buildDiscarder(logRotator(numToKeepStr: '3', artifactNumToKeepStr: '3'))
    }

<<<<<<< HEAD
    stages {

        stage('Setup') {
            steps {
                sh 'make setup'
            }
        }

        stage('Build') {
            steps {
                sh 'make LibCarla'
                sh 'make PythonAPI'
                sh 'make TrafficManager'
                sh 'make CarlaUE4Editor'
                sh 'make examples'
            }
            post {
                always {
                    archiveArtifacts 'PythonAPI/carla/dist/*.egg'
                }
            }
        }

        stage('Unit Tests') {
            steps {
                sh 'make check ARGS="--all --xml"'
            }
            post {
                always {
                    junit 'Build/test-results/*.xml'
                    archiveArtifacts 'profiler.csv'
                }
            }
        }

        stage('Retrieve Content') {
            steps {
                sh './Update.sh'
            }
        }

        stage('Package') {
            steps {
                sh 'make package'
                sh 'make package ARGS="--packages=Town06,Town07 --clean-intermediate"'
            }
            post {
                always {
                    archiveArtifacts 'Dist/*.tar.gz'
=======
    stages
    {
        stage('Creating nodes')
        {
            agent { label "master" }
            steps
            {
                script
                {
                    JOB_ID = "${env.BUILD_TAG}"
                    jenkinsLib = load("/home/jenkins/jenkins.groovy")
                    
                    jenkinsLib.CreateUbuntuBuildNode(JOB_ID)
                    jenkinsLib.CreateWindowsBuildNode(JOB_ID)
                }
            }
        }
        stage('Building CARLA')
        {
            parallel
            {
                stage('ubuntu')
                {
                    agent { label "ubuntu && build && ${JOB_ID}" }
                    environment
                    {
                        UE4_ROOT = '/home/jenkins/UnrealEngine_4.24'
                    }
                    stages
                    {
                        stage('ubuntu setup')
                        {
                            steps
                            {
                                sh 'make setup'
                            }
                        }
                        stage('ubuntu build')
                        {
                            steps
                            {
                                sh 'make LibCarla'
                                sh 'make PythonAPI'
                                sh 'make CarlaUE4Editor'
                                sh 'make examples'
                            }
                            post
                            {
                                always
                                {
                                    archiveArtifacts 'PythonAPI/carla/dist/*.egg'
                                    stash includes: 'PythonAPI/carla/dist/*.egg', name: 'ubuntu_eggs'
                                }
                            }
                        }
                        stage('ubuntu unit tests')
                        {
                            steps
                            {
                                sh 'make check ARGS="--all --xml"'
                            }
                            post
                            {
                                always
                                {
                                    junit 'Build/test-results/*.xml'
                                    archiveArtifacts 'profiler.csv'
                                }
                            }
                        }
                        stage('ubuntu retrieve content')
                        {
                            steps
                            {
                                sh './Update.sh'
                            }
                        }
                        stage('ubuntu package')
                        {
                            steps
                            {
                                sh 'make package'
                                sh 'make package ARGS="--packages=AdditionalMaps --clean-intermediate"'
                                sh 'make examples ARGS="localhost 3654"'
                            }
                            post 
                            {
                                always 
                                {
                                    archiveArtifacts 'Dist/*.tar.gz'
                                    stash includes: 'Dist/CARLA*.tar.gz', name: 'ubuntu_package'
                                    stash includes: 'Examples/', name: 'ubuntu_examples'
                                }
                                success
                                {
                                    node('master')
                                    {
                                        script
                                        {
                                            JOB_ID = "${env.BUILD_TAG}"
                                            jenkinsLib = load("/home/jenkins/jenkins.groovy")
                                            
                                            jenkinsLib.CreateUbuntuTestNode(JOB_ID)
                                        }
                                    }
                                }
                            }
                        }
                        stage('ubuntu smoke tests')
                        {
                            agent { label "ubuntu && gpu && ${JOB_ID}" }
                            steps
                            {
                                unstash name: 'ubuntu_eggs'
                                unstash name: 'ubuntu_package'
                                unstash name: 'ubuntu_examples'
                                sh 'tar -xvzf Dist/CARLA*.tar.gz -C Dist/'
                                sh 'DISPLAY= ./Dist/CarlaUE4.sh -opengl --carla-rpc-port=3654 --carla-streaming-port=0 -nosound > CarlaUE4.log &'
                                sh 'make smoke_tests ARGS="--xml"'
                                sh 'make run-examples ARGS="localhost 3654"'
                            }
                            post
                            {
                                always
                                {
                                    archiveArtifacts 'CarlaUE4.log'
                                    junit 'Build/test-results/smoke-tests-*.xml'
                                    deleteDir()
                                    node('master')
                                    {
                                        script
                                        {
                                            JOB_ID = "${env.BUILD_TAG}"
                                            jenkinsLib = load("/home/jenkins/jenkins.groovy")
                                            
                                            jenkinsLib.DeleteUbuntuTestNode(JOB_ID)
                                        }
                                    }
                                }
                            }
                        }
                        stage('ubuntu deploy')
                        {
                            when { anyOf { branch "master"; buildingTag() } }
                            steps
                            {
                                sh 'git checkout .'
                                sh 'make deploy ARGS="--replace-latest --docker-push"'
                            }
                        }
                        stage('ubuntu Doxygen')
                        {
                            when { anyOf { branch "master"; buildingTag() } }
                            steps
                            {
                                sh 'rm -rf ~/carla-simulator.github.io/Doxygen'
                                sh '''
                                    cd ~/carla-simulator.github.io
                                    git fetch
                                    git checkout -B master origin/master
                                '''
                                sh 'make docs'
                                sh 'cp -rf ./Doxygen ~/carla-simulator.github.io/'
                                sh '''
                                    cd ~/carla-simulator.github.io
                                    git add Doxygen
                                    git commit -m "Updated c++ docs" || true
                                    git push
                                '''
                            }
                            post
                            {
                                always
                                {
                                    deleteDir()
                                }
                            }
                        }
                    }
                    post 
                    {
                        always 
                        { 
                            deleteDir() 

                            node('master')
                            {
                                script
                                {
                                    JOB_ID = "${env.BUILD_TAG}"
                                    jenkinsLib = load("/home/jenkins/jenkins.groovy")
                                    
                                    jenkinsLib.DeleteUbuntuBuildNode(JOB_ID)
                                }
                            }
                        }
                    }
                }
                stage('windows')
                {
                    agent { label "windows && build && ${JOB_ID}" }
                    environment
                    {
                        UE4_ROOT = 'C:\\Program Files\\Epic Games\\UE_4.24'
                    }
                    stages
                    {
                        stage('windows setup')
                        {
                            steps
                            {
                                bat """
                                    call ../setEnv64.bat
                                    make setup
                                """
                            }
                        }
                        stage('windows build')
                        {
                            steps
                            {
                                bat """
                                    call ../setEnv64.bat
                                    make LibCarla
                                """
                                bat """
                                    call ../setEnv64.bat
                                    make PythonAPI
                                """
                                bat """
                                    call ../setEnv64.bat
                                    make CarlaUE4Editor
                                """
                            }
                            post
                            {
                                always
                                {
                                    archiveArtifacts 'PythonAPI/carla/dist/*.egg'
                                    stash includes: 'PythonAPI/carla/dist/*.egg', name: 'windows_eggs'
                                }
                            }
                        }
                        stage('windows retrieve content')
                        {
                            steps
                            {
                                bat """
                                    call ../setEnv64.bat
                                    call Update.bat
                                """
                            }
                        }
                        stage('windows package')
                        {
                            steps
                            {
                                bat """
                                    call ../setEnv64.bat
                                    make package
                                """
                                bat """
                                    call ../setEnv64.bat
                                    make package ARGS="--packages=AdditionalMaps --clean-intermediate"
                                """
                            }
                            post {
                                always {
                                    archiveArtifacts 'Build/UE4Carla/*.zip'
                                }
                            }
                        }
                        stage('windows deploy')
                        {
                            when { anyOf { branch "master"; buildingTag() } }
                            steps {
                                bat """
                                    call ../setEnv64.bat
                                    git checkout .
                                    make deploy ARGS="--replace-latest"
                                """
                            }
                        }
                    }
                    post 
                    {
                        always 
                        { 
                            deleteDir() 

                            node('master')
                            {
                                script
                                {
                                    JOB_ID = "${env.BUILD_TAG}"
                                    jenkinsLib = load("/home/jenkins/jenkins.groovy")
                                    
                                    jenkinsLib.DeleteWindowsBuildNode(JOB_ID)
                                }
                            }
                        }
                    }
>>>>>>> 4dc4cb81853670d83ee067ae747c8c851926dacd
                }
            }
        }

        stage('Smoke Tests') {
            steps {
                sh 'DISPLAY= ./Dist/*/LinuxNoEditor/CarlaUE4.sh -opengl --carla-rpc-port=3654 --carla-streaming-port=0 -nosound > CarlaUE4.log &'
                sh 'make smoke_tests ARGS="--xml"'
                sh 'make run-examples ARGS="localhost 3654"'
            }
            post {
                always {
                    archiveArtifacts 'CarlaUE4.log'
                    junit 'Build/test-results/smoke-tests-*.xml'
                }
            }
        }

        stage('Deploy') {
            when { anyOf { branch "master"; buildingTag() } }
            steps {
                sh 'make deploy ARGS="--replace-latest --docker-push"'
            }
        }

        stage('Doxygen') {
            when { anyOf { branch "master"; buildingTag() } }
            steps {
                sh 'make docs'
                sh 'rm -rf ~/carla-simulator.github.io/Doxygen'
                sh 'cp -rf ./Doxygen ~/carla-simulator.github.io/'
                sh 'cd ~/carla-simulator.github.io && \
                    git pull && \
                    git add Doxygen && \
                    git commit -m "Updated c++ docs" || true && \
                    git push'
            }
        }
    }

    post {
        always {
            deleteDir()
        }
    }
}
