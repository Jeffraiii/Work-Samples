using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FreezeBlockScript : MonoBehaviour
{



    private Rigidbody rb;

    // Start is called before the first frame update
    void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {

    }


    private void OnCollisionEnter(Collision other)
    {
        if (other.collider.CompareTag("Ground") || other.collider.CompareTag("Stackable"))
        {
            gameObject.GetComponent<Rigidbody>().angularVelocity = Vector3.zero;

            gameObject.GetComponent<Rigidbody>().isKinematic = true;


        }


    }
}
