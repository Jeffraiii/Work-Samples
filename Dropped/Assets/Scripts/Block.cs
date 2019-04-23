using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Block : MonoBehaviour
{
    public float lifeTime;

    private Rigidbody rb;
    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
        GetComponent<TrailRenderer>().enabled = false;
    }

    // Update is called once per frame
    void Update()
    {
        lifeTime -= Time.deltaTime;
        if (lifeTime < 0.0f)
        {
            Destroy(gameObject);
        }
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.CompareTag("GravityWell"))
        {
            if (GetComponent<HingeJoint>() == null)
            {
                HingeJoint hinge = gameObject.AddComponent<HingeJoint>();
                hinge.anchor = transform.InverseTransformPoint(other.gameObject.transform.position);
                hinge.axis = Vector3.Cross(other.gameObject.transform.position - transform.position, Vector3.right).normalized;
                lifeTime += 10.0f;
            }
        }
   
    }

    private void OnTriggerStay(Collider other)
    {
//        if (Vector3.Dot(rb.velocity, Vector3.Cross(other.transform.position - transform.position, Vector3.right)) > 0)
//        {
//            rb.AddForce(Vector3.Cross(other.transform.position - transform.position, Vector3.right).normalized * 3);
//        }
//        else
//        {
//            rb.AddForce(Vector3.Cross(other.transform.position - transform.position, Vector3.right).normalized * -3);
//
//        }
        if (other.gameObject.CompareTag("GravityWell"))
        {
            GetComponent<TrailRenderer>().enabled = true;
            if (other.gameObject.GetComponent<InitWell>().explode)
            {
                Destroy(GetComponent<HingeJoint>());
                rb.AddForce(transform.position - other.transform.position, ForceMode.Impulse);
                //Destroy(other.gameObject);
                return;
            }
            rb.AddForce(Vector3.Cross(other.transform.position - transform.position, GetComponent<HingeJoint>().axis).normalized * 10);
        }

        //rb.AddTorque(new Vector3(50,0,0));
    }
}
