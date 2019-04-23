using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ConstantSpawner : MonoBehaviour
{
    private GameObject p;
    // Start is called before the first frame update
    void Start()
    {
        p = GameObject.FindGameObjectWithTag("Player");
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 temp = transform.position;
        temp.z = p.transform.position.z + 70.0f;
        transform.position = temp;
    }
}
