using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DrawGizmos : MonoBehaviour {

    [SerializeField]
    float debugDrawRadius = 0.25F;

    void OnDrawGizmos()
    {
        Gizmos.color = Color.red;
        Gizmos.DrawWireSphere(transform.position, debugDrawRadius);
    }
}
