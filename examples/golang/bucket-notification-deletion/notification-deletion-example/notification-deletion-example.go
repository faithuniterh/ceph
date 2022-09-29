package main

import (
	"flag"
	"fmt"
	"os"
        "notification"

	"github.com/aws/aws-sdk-go/aws"
	"github.com/aws/aws-sdk-go/aws/credentials"
	"github.com/aws/aws-sdk-go/aws/endpoints"
	"github.com/aws/aws-sdk-go/aws/session"
	"github.com/aws/aws-sdk-go/service/s3"
)

func main() {
	bucket := flag.String("b", "", "Name of the bucket to remove notification from")
	notificationID := flag.String("t", "", "The notification identifier for a specific notification")
	flag.Parse()

	if *bucket == "" {
		fmt.Println("You must supply the name of the bucket")
		fmt.Println("-b BUCKET")
		return
	}

	if *notificationID == "" {
		fmt.Println("You must supply the ID of the notification ")
		fmt.Println("-t NOTIFICATION ID")
		return
	}

	//Ceph RGW Credentials
	access_key := "0555b35654ad1656d804"
	secret_key := "h7GhxuBLTrlhVUyxSPUKUV8r/2EI4ngqJxD7iBdBYLhwluN30JaT3Q=="
	url := "http://127.0.0.1:8000"

	defaultResolver := endpoints.DefaultResolver()
	CustResolverFn := func(service, region string, optFns ...func(*endpoints.Options)) (endpoints.ResolvedEndpoint, error) {
		if service == "s3" {
			return endpoints.ResolvedEndpoint{
				URL: url,
			}, nil
		}

		return defaultResolver.EndpointFor(service, region, optFns...)
	}

	sess := session.Must(session.NewSessionWithOptions(session.Options{
		Config: aws.Config{
			Region:           aws.String("default"),
			Credentials:      credentials.NewStaticCredentials(access_key, secret_key, ""),
			S3ForcePathStyle: aws.Bool(true),
			EndpointResolver: endpoints.ResolverFunc(CustResolverFn),
		},
	}))

	svc := s3.New(sess)

	input := &notification.DeleteBucketNotificationRequestInput{
		Bucket: bucket,
	}

	_, err := notification.DeleteBucketNotification(svc, input)

	if err != nil {
		exitErrorf("Unable to delete Put Bucket Notification because of %s", err)
	}
	fmt.Println("Put bucket notification added to  ", *topic)
}

func exitErrorf(msg string, args ...interface{}) {
	fmt.Fprintf(os.Stderr, msg+"\n", args...)
	os.Exit(1)
}
