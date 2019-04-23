using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraScript : MonoBehaviour
{
    public GameObject objectToFollow;

    public float speed = 2.0f;

    public float yspeed = 100.0f;

    public float xOffset = 0;
    public float yOffset = 2.0f;
    public float zOffset = -1.0f;



    private Vector3 _velocity = Vector3.zero;


    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void FixedUpdate()
    {
        //float interpolation = speed * Time.deltaTime;


        Vector3 playerPos = objectToFollow.transform.position;
        playerPos = playerPos + Vector3.forward * zOffset + Vector3.up * yOffset;
//
        Vector3 position = this.transform.position;
        position.x = Mathf.Lerp(this.transform.position.x, (objectToFollow.transform.position.x + xOffset), speed * Time.deltaTime);
        position.y = Mathf.Lerp(this.transform.position.y, (objectToFollow.transform.position.y + yOffset), yspeed * Time.deltaTime);
        position.z = Mathf.Lerp(this.transform.position.z, (objectToFollow.transform.position.z + zOffset), speed * Time.deltaTime);

        //transform.position = Vector3.Lerp(this.transform.position, objectToFollow.transform.position, interpolation);

        //Vector3 position = objectToFollow.transform.position - 10.0f * objectToFollow.transform.forward;

        transform.position = position;

        //this.gameObject.transform.position = Vector3.SmoothDamp(this.gameObject.transform.position, playerPos, ref _velocity, cameraSpeed);
        //change the last value to the time you want to complete the movement.


        //transform.position = objectToFollow.transform.position - 10.0f * objectToFollow.transform.forward;
    }
}
